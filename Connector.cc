#include<unistd.h>
#include"Connector.h"
#include"EventLoop.h"
using namespace conex;

namespace
{
int createNonblocking()
{
    int ret = ::socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(ret < 0)
    {
        //LOG_FATAL
    }
    return ret;
}
}
Connector::Connector(EventLoop *loop,const InetAddress& peer)
    : loop_(loop)
    , sockfd_(createNonblocking())
    , channel_(loop,sockfd_)
    , peer_(peer)
    , connected_(false)
    , started_(false)
{
    channel_.setWriteCallback(
        std::bind(&Connector::handleWrite,this)
    );
}
Connector::~Connector()
{
    if(!connected_)
    {
        ::close(sockfd_);
    }
}
void Connector::start()
{
    started_ = true;

    int ret = ::connect(sockfd_,peer_.getSockAddr(),peer_.getSockLen());
    if( ret == -1 )
    {
        if(errno != EINPROGRESS)
            handleWrite();
        else
            channel_.enableWriting();
    }
    else 
        handleWrite();
}
void Connector::handleWrite()
{
    loop_->removeChannel(&channel_);//为何要先移除呢

    int err;
    socklen_t len = sizeof(err);
    int ret = ::getsockopt(sockfd_,SOL_SOCKET,SO_ERROR,&err,&len);
    if(ret == 0)
        errno = err;
    if(errno != 0)
    {
        //LOG_ERROR
        if(errorCallback_)
        {
            errorCallback_();
        }
    }
    else if (newConnectionCallback_)
    {
        sockaddr_in localAddr;
        len = sizeof(localAddr);
        ret = ::getsockname(sockfd_,(sockaddr*)(&localAddr),&len);
        if(ret == -1)
        {
            //LOG_ERR
        }
        InetAddress local(localAddr);
        // now the fd not belong to us
        connected_ = true;
        newConnectionCallback_(sockfd_,local,peer_);
    }
    
}