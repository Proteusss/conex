/**
 * @file EventLoop.h
 * @author your name (you@domain.com)
 * @brief 
 * @date 2021-03-29
 */
#ifndef CONEX_EVENTLOOP_H
#define CONEX_EVENTLOOP_H
#include<atomic>
#include<mutex>

#include"Timer.h"
#include"EPoller.h"
#include"TimerQueue.h"

namespace conex
{

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    //线程安全
    void quit(); 
    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    //void assertInLoopThread();
    //void assertNotInLoopThread();
    bool isInLoopThread();

    void runInLoop(const Task& task);
    void runInLoop(Task&& task);
    void queueInLoop(const Task& task);
    void queueInLoop(Task&& task);

    Timer* runAt(Timestamp when, TimerCallback callback);
    Timer* runAfter(NanoSecond interval, TimerCallback callback);
    Timer* runEvery(NanoSecond interval, TimerCallback callback);
    void cancelTimer(Timer* timer);


private:
    void doPendingTasks();
    void handleRead();

    std::atomic_bool quit_;

    EPoller poller_;
    EPoller::ChannelList activeChannels_;

    const int wakeupFd_;
    Channel wakeupChannel_;

    bool doingPendingTasks_;
    std::mutex mutex_;
    std::vector<Task> pendingTasks_;

    const pid_t tid_;
    TimerQueue timerQueue_;

};
}
#endif