/**
 * @file TcpClient.h
 * @author zjb
 * @brief
 * @date 2021-04-05
 */
#ifndef CONEX_TCPCLIENT_H
#define CONEX_TCPCLIENT_H
#include<memory.h>

#include "noncopyable.h"
#include "Connector.h"
#include "InetAddress.h"
#include "Timer.h"
namespace conex
{
class TcpClient : noncopyable
{
public:
    TcpClient(EventLoop *loop, const InetAddress& peer);
    ~TcpClient();

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
    void setErrorCallback(const ErrorCallback& cb)
    { connector_->setErrorCallback(cb); }
    
    void start();

private:
    void newConnection(int connfd, const InetAddress& local, const InetAddress& peer);
    void closeConnection(const TcpConnectionPtr& conn);
    void retry();
    using ConnectorPtr = std::unique_ptr<Connector>;

    EventLoop *loop_;
    const InetAddress peer_;
    ConnectorPtr connector_;
    TcpConnectionPtr connection_;
    bool connected_;
    Timer *retryTimer_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    WriteCompleteCallback writeCompleteCallback_;


};
}

#endif