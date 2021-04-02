/**
 * @file EventLoop.cc
 * @author zjb
 * @brief 
 * @date 2021-03-31
 */
#include<syscall.h> //SYS_gettid
#include<unistd.h> //syscall
#include<sys/eventfd.h>
#include"EventLoop.h"

using namespace conex;

namespace
{
__thread EventLoop* t_loop = nullptr;

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

}

EventLoop::EventLoop()
    : quit_(false)
    , poller_(this)
    , wakeupFd_(::eventfd(0,EFD_CLOEXEC | EFD_NONBLOCK))
    , wakeupChannel_(this,wakeupFd_)
    , doingPendingTasks_(false)
    , tid_(gettid())
    , timerQueue_(this)
{
    if(wakeupFd_ < 0)
    {
        //LOG_FATAL
    }

    wakeupChannel_.setReadCallback(std::bind(&EventLoop::handleRead,this));
    wakeupChannel_.enableReading();

    t_loop = this;
}
EventLoop::~EventLoop()
{
    t_loop = nullptr;
}
void EventLoop::loop()
{
    //LOG_INFO
    quit_ = false;
    while(quit_)
    {
        activeChannels_.clear();
        Timestamp poolReturnTime = poller_.poll(activeChannels_);
        for(auto channel: activeChannels_)
            channel->handleEvents(poolReturnTime);
        doPendingTasks();
    }
    //LOG_INFO
}
void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}
void EventLoop::runInLoop(const Task& task)
{
    if(isInLoopThread())
    {
        task();
    }
    else
    {
        queueInLoop(task);
    }
}
void EventLoop::runInLoop(Task&& task)
{
    if(isInLoopThread())
    {
        task();
    }
    else
    {
        queueInLoop(std::move(task));
    }
}
void EventLoop::queueInLoop(const Task& task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingTasks_.push_back(task);
    }
    if(!isInLoopThread() || doingPendingTasks_ )
        wakeup();
}
void EventLoop::queueInLoop(Task&& task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingTasks_.push_back(std::move(task));
    }
    if(!isInLoopThread() || doingPendingTasks_)
        wakeup();
}
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_,&one, sizeof(one) );
    if(n != sizeof(one))
    {
        //LOG_ERROR
    }
}
void EventLoop::updateChannel(Channel* channel)
{
    poller_.updateChannel(channel);
}
void EventLoop::removeChannel(Channel* channel)
{
    channel->disableAll();
}
bool EventLoop::isInLoopThread()
{
    return tid_ == gettid();
}
void EventLoop::doPendingTasks()
{
    std::vector<Task> tasks;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks.swap(pendingTasks_);
    }
    doingPendingTasks_ = true;
    for(auto& task : tasks)
    {
        task();
    }
    doingPendingTasks_ = false;
}
void EventLoop::handleRead()
{
    uint64_t one;
    ssize_t n = ::read(wakeupFd_,&one,sizeof(one));
    if(n != sizeof(one))
    {
        //LOG_ERROR
    }
}