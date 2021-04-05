/**
 * @file TcpServerSingle.h
 * @author zjb
 * @brief 
 * @date 2021-04-04
 */
#ifndef CONEX_TCPSERVERSINGLE_H
#define CONEX_TCPSERVERSINGLE_H
#include <unordered_set>

#include "noncopyable.h"
#include "Acceptor.h"

namespace conex
{
class EventLoop;
class TcpServerSingle : noncopyable
{
public:
    TcpServerSingle(EventLoop *loop, const InetAddress& local);

    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = std::move(cb); } //之后cb内存中的内容就被移走了
    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = std::move(cb); }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = std::move(cb); }
private:
    void newConnection(int connfd,const InetAddress& local, const InetAddress& peer);
    void closeConnection(const TcpConnectionPtr& conn);

    using ConnectionSet = std::unordered_set<TcpConnectionPtr>;

    EventLoop *loop_;
    Acceptor acceptor_;
    ConnectionSet connections_;

    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    WriteCompleteCallback writeCompleteCallback_;
};
}

#endif