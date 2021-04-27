/**
 * @file TcpConnection.h
 * @author zjb
 * @brief 
 * @date 2021-04-03
 */
#ifndef CONEX_TCPCONNECTION_H
#define CONEX_TCPCONNECTION_H
#include<atomic>

#include "noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Channel.h"
#include "Buffer.h"
#include "Timestamp.h"
#include "any.h"

namespace conex
{
class EventLoop;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, int sockfd, const InetAddress& local, const InetAddress& peer);
    ~TcpConnection();

    const InetAddress& local() const { return local_; }
    const InetAddress& peer() const { return peer_; }
    const Buffer& inputBuffer() const { return inputBuffer_;}
    const Buffer& outputBuffer() const { return outputBuffer_;}
    std::string name() const
    {
        return peer_.toIpPort() + " -> " + local_.toIpPort();
    }

    bool connected() const
    { return state_ == kConnected; }
    bool disconnected() const
    { return state_ == kDisconnected; }

    //用户设置
    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb;}
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCall& cb, size_t mark)
    { highWaterMarkCallback_ = cb; highWaterMark_ = mark;}
    //内部设置
    void setCloseCallBack(const CloseCallback& cb)
    { closeCallback_ = cb; }

    void connectEstablished();

    //IO操作，线程安全
    void send(const std::string& msg);
    void send(const char* data, size_t len);
    void send(Buffer& buffer);
    void shutdown();
    void forceClose();

    void stopRead();
    void startRead();
    bool isReading() const { return channel_.isReading(); }

    void setContext(const any& context)
    { context_ = context; }
    const any& getContext() const
    { return context_; }
    any& getContext()
    { return context_; }

private:
    enum ConnectionState
    {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    };
    void setState(ConnectionState state) { state_ = state;}
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const char* data, size_t len);
    void sendInLoop(const std::string& message);
    void shutdownInLoop();
    void forceCloseInLoop();



    EventLoop *loop_;
    const int sockfd_;
    Channel channel_; //FIXEME:可能需要改为使用unique_ptr来控制
    std::atomic_int state_;
    InetAddress local_;
    InetAddress peer_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    size_t highWaterMark_;

    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCall highWaterMarkCallback_;
    CloseCallback closeCallback_;

    any context_;
};

}

#endif