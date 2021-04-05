#include<unistd.h> //close
#include"Acceptor.h"
#include"EventLoop.h"

using namespace conex;

namespace //FIXME: 考虑一下用匿名名称空间还是static函数？
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

Acceptor::Acceptor(EventLoop *loop,const InetAddress& local)
    : loop_(loop)
    , acceptFd_(createNonblocking())
    , acceptChannel_(loop,acceptFd_)
    , local_(local)
    , listening_(false)
{
    int on =1;
    //以下设置将port和addr重用，让内核进行负载均衡
    int ret = ::setsockopt(acceptFd_,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret < 0) 
    { 
        //LOG_FATAL
    }
    ret = ::setsockopt(acceptFd_,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
    if(ret < 0)
    {
        //LOG_FATAL
    }
    ret = ::bind(acceptFd_,local.getSockAddr(),local.getSockLen());
    if(ret < 0)
    {
        //LOG_FATAL
    }
}
Acceptor::~Acceptor()
{
    ::close(acceptFd_);
}
void Acceptor::listen()
{
    int ret = ::listen(acceptFd_,SOMAXCONN);
    if(ret < 0)
    {
        //LOG_FATAL
    }
    acceptChannel_.setReadCallback(
        std::bind(&Acceptor::handleRead,this) //这里的timestamp参数跑哪去了？
    );
    acceptChannel_.enableReading();
}
void Acceptor::handleRead()
{
    sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr);
    int connfd = ::accept4(acceptFd_,(sockaddr*)(&peerAddr),&len,
                                    SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd == -1)
    {
        int savedErrno = errno;
        //LOG_ERROR
        switch (savedErrno)
        {
            case ECONNABORTED:
            case EMFILE:
                break;
            default:
                //LOG_FATAL
                break;
        }
    }
    if(newCoonectionCallback_)
    {
        InetAddress peer(peerAddr);
        newCoonectionCallback_(connfd,local_,peer);
    }
    else
    {
        ::close(connfd);
    }

}