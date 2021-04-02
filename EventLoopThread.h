/**
 * @file EventLoopThread.h
 * @author zjb
 * @brief 
 * @date 2021-03-31
 */

#ifndef CONEX_EVENTLOOPTHREAD_H
#define CONEX_EVENTLOOPTHREAD_H

#include<thread>

#include"CountDownLatch.h"

namespace conex
{
class EventLoop;

class EventLoopThread : noncopyable
{
public:
    EventLoopThread()
        : loop_(nullptr)
        , thread_()
        , started_(false)
    {
    }
    ~EventLoopThread();

    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop *loop_;
    std::thread thread_;
    bool started_;
    CountDownLatch latch_{1};
};

}

#endif