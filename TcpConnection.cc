#include<unistd.h>

#include"TcpConnection.h"
#include"EventLoop.h"

using namespace conex;

namespace
{
EventLoop* CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        //LOG_FATAL("%s:%s:%d TcpConnection Loop is null! \n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}
}

namespace conex
{

//默认回调，用于日志记录调用过程


}
TcpConnection::TcpConnection(EventLoop *loop, int sockfd, const InetAddress& local, const InetAddress& peer)
    : loop_(CheckLoopNotNull(loop))
    , sockfd_(sockfd)
    , channel_(loop, sockfd)
    , state_(kConnecting)
    , local_(local)
    , peer_(peer)
    , highWaterMark_(0)
{
    channel_.setReadCallback(
        std::bind(&TcpConnection::handleRead,this,std::placeholders::_1)
    );
    channel_.setWriteCallback(
        std::bind(&TcpConnection::handleWrite,this)
    );
    channel_.setCloseCallback(
        std::bind(&TcpConnection::handleClose,this)
    );
    channel_.setErrorCallback(
        std::bind(&TcpConnection::handleError,this)
    );

    //LOG_INFO
}
TcpConnection::~TcpConnection()
{
    ::close(sockfd_);
    //LOG_INFO
}
void TcpConnection::connectEstablished()
{
    state_ = kConnected;
    channel_.tie(shared_from_this());
    channel_.enableReading();
}

//IO操作
void TcpConnection::send(const std::string& msg)
{
    send(msg.data(),msg.size());
}
void TcpConnection::send(const char *data, size_t len)
{
    if(state_ != kConnected)
    {
        //LOG_WARN
        return;
    }
    if(loop_->isInLoopThread())
    {
        sendInLoop(data,len);
    }
    else
    {
        loop_->queueInLoop(
           // 用下面这个绑定器会报错
           //std::bind(&TcpConnection::sendInLoop, this, std::string(data,data+len))
           [ptr = shared_from_this(), str = std::string(data,data+len)]()
           { ptr->sendInLoop(str);}
        );
    }
}
void TcpConnection::sendInLoop(const std::string& msg)
{
    sendInLoop(msg.data(), msg.size());
}
void TcpConnection::sendInLoop(const char *data,size_t len)
{
    if(state_ != kConnected)
    {
        //LOG_WARN
        return;
    }

    ssize_t nwrote = 0;
    size_t remain = len;
    bool faultError = false;
    if(!channel_.isWriting() && outputBuffer_.readableBytes() == 0) //没有之前遗留的数据需要发送
    {
        nwrote = ::write(channel_.fd(),data,len);
        if(nwrote == -1)
        {
            if(errno != EAGAIN)
            {
                //LOG_ERROR
                if(errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
            nwrote = 0;
        }
        else
        {
            remain = len - static_cast<size_t>(nwrote);
            if(remain == 0 && writeCompleteCallback_) //本次数据发送完毕
            {
                loop_->queueInLoop(
                    std::bind(writeCompleteCallback_,shared_from_this())
                );
            }
        }
        if(!faultError && remain > 0) //本次未发送完，需将剩余数据输出至缓冲区，并给channel向poller注册可写事件
        {
            size_t oldLen = outputBuffer_.readableBytes();
            if(oldLen + remain >= highWaterMark_
                && oldLen < highWaterMark_ && highWaterMarkCallback_) // 触发水位线回调
            {
                loop_->queueInLoop(
                    std::bind(highWaterMarkCallback_,shared_from_this(),oldLen + remain)
                );
            }

            outputBuffer_.append(data+nwrote,remain);
            if(!channel_.isWriting())
            {
                channel_.enableWriting();
            }
        }

    }
}
void TcpConnection::send(Buffer& buffer)
{
    if(state_ != kConnected)
    {
        //LOG_WARN
        return;
    }
    if(loop_->isInLoopThread())
    {
        sendInLoop(buffer.peek(),buffer.readableBytes());
        buffer.retrieveAll();
    }
    else
    {
        loop_->queueInLoop(
           // std::bind(&TcpConnection::sendInLoop,this,buffer.retrieveAllAsString())
           [ptr = shared_from_this(), str = buffer.retrieveAllAsString()]()
           { ptr->sendInLoop(str); }
        );
    }
}
void TcpConnection::shutdown()
{
    if(state_ == kConnected)
    {
        setState(kDisconnecting);
        if(loop_->isInLoopThread())
            shutdownInLoop();
        else
        {
            loop_->queueInLoop(
                std::bind(&TcpConnection::shutdownInLoop,shared_from_this())
            );
        }
    }
}
void TcpConnection::shutdownInLoop()
{
    if(state_ != kDisconnected && !channel_.isWriting())
    {
        if(::shutdown(sockfd_,SHUT_WR) == -1)
        {
            //LOG_ERROR
        }
    }
}
void TcpConnection::forceClose()
{
    if(state_ != kDisconnected)
    {
        setState(kDisconnecting);
        loop_->queueInLoop(
            std::bind(&TcpConnection::forceCloseInLoop,shared_from_this())
        );
    }
}
void TcpConnection::forceCloseInLoop()
{
    if(state_ != kDisconnected)
    {
        handleClose();
    }
}
void TcpConnection::stopRead()
{
    loop_->runInLoop([this](){
        if(channel_.isReading())
            channel_.disableReading();
    });
}
void TcpConnection::startRead()
{
    loop_->runInLoop([this](){
        if(!channel_.isReading())
        {
            channel_.enableReading();
        }
    });
}
void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_.fd(),&savedErrno);
    if( n > 0)
    {
        messageCallback_(shared_from_this(),inputBuffer_,receiveTime);
    }
    else if (n==0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        //LOG_ERROR
        handleError();
    }
}
void TcpConnection::handleWrite()
{
    if(state_ == kDisconnected)
    {
        //LOG_WARN
        return;
    }
    int savedErrno;
    ssize_t n = outputBuffer_.writeFd(channel_.fd(),&savedErrno);
    if( n == -1)
    {
        //LOG_ERROR
    }
    else
    {
        outputBuffer_.retrieve(static_cast<size_t>(n));
        //如果发送完毕则取消堆可写事件的关注并调用相应回调
        if(outputBuffer_.readableBytes() == 0)
        {
            channel_.disableWriting();
            if(writeCompleteCallback_)
            {
                loop_->queueInLoop(
                    std::bind(writeCompleteCallback_,shared_from_this())
                );
            }
            if(state_ = kDisconnecting)
            {
                //FIEXME:这里需不需要考虑不在当前所属线程中执行？
                shutdownInLoop();
            }
        }
    }
}
void TcpConnection::handleClose()
{
    setState(kDisconnected);
    loop_->removeChannel(&channel_);
    closeCallback_(shared_from_this());
}
void TcpConnection::handleError()
{
    int err;
    socklen_t len = sizeof(err);
    int ret = ::getsockopt(sockfd_,SOL_SOCKET,SO_ERROR,&err,&len);
    if(ret != -1)
        errno = err;
    //LOG_ERROR;
}