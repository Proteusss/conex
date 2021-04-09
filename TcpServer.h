/**
 * @file TcpServer.h
 * @author your name (you@domain.com)
 * @brief 
 * @date 2021-04-05
 */
#ifndef CONEX_TCPSERVER_H
#define CONEX_TCPSERVER_H

#include<thread>
#include<memory>
#include<vector>
#include<atomic>
#include<mutex>
#include<condition_variable>

#include "noncopyable.h"
#include "TcpServerSingle.h"
#include "InetAddress.h"
#include "Callbacks.h"


namespace conex
{
class EventLoop;
class TcpServerSingle;

class TcpServer : noncopyable   
{
public:
    TcpServer(EventLoop *loop, const InetAddress& local);
    ~TcpServer();

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }
    void setThreadInitCallback(const ThreadInitCallback& cb)
    { threadInitCallback_ = cb; }

    void setThreadNum(size_t n);
    void start();
private:
    void startInLoop();
    void runInThread(size_t index);

    using ThreadPtr = std::unique_ptr<std::thread>;
    using ThreadPtrList = std::vector<ThreadPtr>;
    using EventLoopList = std::vector<EventLoop*>;
    using TcpServerSinglePtr = std::unique_ptr<TcpServerSingle>;

    EventLoop *baseLoop_;
    InetAddress local_;
    TcpServerSinglePtr baseServer_;
    EventLoopList eventloops_;
    ThreadPtrList threads_;
    size_t threadNum_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic_bool started_;

    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ConnectionCallback connectionCallback_;
    ThreadInitCallback threadInitCallback_;
};
}

#endif