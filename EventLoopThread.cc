/**
 * @file EventLoopThread.cc
 * @author your name (you@domain.com)
 * @brief 
 * @date 2021-03-31
 */

#include"EventLoop.h"
#include"EventLoopThread.h"

using namespace conex;

EventLoopThread::~EventLoopThread()
{
    if(started_)
    {
        if(loop_ != nullptr)
        {
            loop_->quit();
        }
        thread_.join();
    }
}
void EventLoopThread::threadFunc()
{
    EventLoop loop;
    loop_ = &loop;
    latch_.count();
    loop.loop();
    loop_ = nullptr;
}
EventLoop* EventLoopThread::startLoop()
{
    started_ = true;
    thread_ = std::thread( std::bind(&EventLoopThread::threadFunc,this));
    latch_.wait(); //等待loop_已经在另一个线程中被设置完成

    return loop_;
}