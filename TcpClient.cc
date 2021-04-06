#include"TcpClient.h"
#include"EventLoop.h"
#include"TcpConnection.h"
#include"Connector.h"

using namespace conex;

TcpClient::TcpClient(EventLoop *loop,const InetAddress& peer)
    : loop_(loop)
    , peer_(peer)
    , connector_(new Connector(loop, peer))
    , connected_(false)
    , retryTimer_(nullptr)
{
    connector_->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection,this,std::placeholders::_1,
                std::placeholders::_2,std::placeholders::_3)
    );
}
TcpClient::~TcpClient()
{
    if(connection_ && !connection_->disconnected())
        connection_->forceClose();
    if(retryTimer_ != nullptr)
    {
        loop_->cancelTimer(retryTimer_);
    }
}
void TcpClient::start()
{
    connector_->start();
    retryTimer_ = loop_->runEvery(3s,
    std::bind(&TcpClient::retry,this)
    );
}
void TcpClient::retry()
{
    if(connected_)
    {
        return;
    }
    connector_ = std::make_unique<Connector>(loop_,peer_);
    connector_ ->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection,this,std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3)
    );
    connector_->start();
}
void TcpClient::newConnection(int connfd,const InetAddress& local, const InetAddress& peer)
{   
    connected_ = true;
    loop_->cancelTimer(retryTimer_);
    retryTimer_ = nullptr;
    auto conn = std::make_shared<TcpConnection>(loop_,connfd,local,peer);
    connection_ = conn;
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallBack(
        std::bind(&TcpClient::closeConnection,this,std::placeholders::_1)
    );
    //enable and tie channel
    conn->connectEstablished();
    connectionCallback_(conn);
}
void TcpClient::closeConnection(const TcpConnectionPtr& conn)
{
    connection_.reset();
    connectionCallback_(conn);
}