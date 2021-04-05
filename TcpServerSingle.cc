#include<memory>
#include"TcpServerSingle.h"
#include"EventLoop.h"
#include"TcpConnection.h"
using namespace conex;

TcpServerSingle::TcpServerSingle(EventLoop *loop, const InetAddress& local)
    : loop_(loop)
    , acceptor_(loop_,local)
{
    acceptor_.setNewConnectionCallback(
        std::bind(&TcpServerSingle::newConnection,this,std::placeholders::_1
                            , std::placeholders::_2, std::placeholders::_2)
    );
}
void TcpServerSingle::start()
{
    acceptor_.listen();
}
void TcpServerSingle::newConnection(int connfd, const InetAddress& local, const InetAddress& peer)
{
    auto conn = std::make_shared<TcpConnection>(loop_,connfd,local,peer);
    connections_.insert(conn);
    //后续messageCallback_确认不会用到了吗，这个set函数里已经把内容移走了，
    //或者setXXXCallback函数里不应该使用std::move
    conn->setMessageCallback(messageCallback_); 
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallBack(
        std::bind(&TcpServerSingle::closeConnection,this,std::placeholders::_1)
    );
    conn->connectEstablished();
    connectionCallback_(conn);
}   
void TcpServerSingle::closeConnection(const TcpConnectionPtr& conn)
{
    connections_.erase(conn);
    connectionCallback_(conn);
}