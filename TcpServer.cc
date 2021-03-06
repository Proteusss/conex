#include <thread>
#include<cstdio>
#include "TcpServer.h"
#include "EventLoop.h"
#include"TcpConnection.h"

using namespace conex;
namespace conex
{
void defaultThreadInitCallback(size_t index)
{
    printf("EventLoop thread #%lu started", index);
}

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    printf("connection %s -> %s %s",
         conn->peer().toIpPort().c_str(),
         conn->local().toIpPort().c_str(),
         conn->connected() ? "up" : "down");
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer& buffer,Timestamp time)
{
    printf("connection %s -> %s recv %lu bytes",
          conn->peer().toIpPort().c_str(),
          conn->local().toIpPort().c_str(),
          buffer.readableBytes());
    buffer.retrieveAll();
}
}
TcpServer::TcpServer(EventLoop *loop, const InetAddress& local)
    : baseLoop_(loop)
    , local_(local)
    , threadNum_(1)
    , started_(false)
    , threadInitCallback_(defaultThreadInitCallback)  //不设置默认的回调函数，而用户又没设置回调函数时，运行会报错
    , messageCallback_(defaultMessageCallback)
    , connectionCallback_(defaultConnectionCallback)
{
    //LOG_INFO
    printf("create TcpServer() %s\n", local.toIpPort().c_str());
}

TcpServer::~TcpServer()
{
    for(auto& loop : eventloops_)
    {
        if(loop != nullptr)
            loop->quit();
    }
    for(auto& thread : threads_)
    {
        thread->join();
    }
    //LOG_INFO
}
void TcpServer::setThreadNum(size_t n)
{
    if(!started_)
    {
        threadNum_ = n;
        eventloops_.resize(n);
    }
    else
    {
        //LOG_ERROR
    }
}
void TcpServer::start()
{
    if(started_.exchange(true))
        return; // 之前已启动
    
    baseLoop_->runInLoop(
        std::bind(&TcpServer::startInLoop,this)
    );
}
void TcpServer::startInLoop()
{
    //LOG_INFO

    baseServer_ = std::make_unique<TcpServerSingle>(baseLoop_,local_);
    baseServer_->setMessageCallback(messageCallback_);
    baseServer_->setConnectionCallback(connectionCallback_);
    baseServer_->setWriteCompleteCallback(writeCompleteCallback_);
    threadInitCallback_(0);
    baseServer_->start();

    for(size_t i = 1; i < threadNum_; ++i)
    {
        auto thread = new std::thread(
            std::bind(&TcpServer::runInThread,this,i)
        );
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(eventloops_[i] == nullptr)
                cond_.wait(lock);
        }
        threads_.emplace_back(thread);
    }
    //baseLoop没有开启loop() ，需要在用户程序中开启。
}
void TcpServer::runInThread(size_t index)
{
    EventLoop loop;
    TcpServerSingle server(&loop, local_);
    server.setMessageCallback(messageCallback_);
    server.setConnectionCallback(connectionCallback_);
    server.setWriteCompleteCallback(writeCompleteCallback_);

    {
        std::lock_guard<std::mutex> guard(mutex_);
        eventloops_[index] = &loop;
        cond_.notify_one();
    }

    threadInitCallback_(index);
    server.start();
    loop.loop();
    eventloops_[index] = nullptr; // 该loop已退出
}