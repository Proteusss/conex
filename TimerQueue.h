/**
 * @file TimerQueue.h
 * @author your name (you@domain.com)
 * @brief 
 * @date 2021-03-30
 */
#ifndef CONEX_TIMERQUEUE_H
#define CONEX_TIMERQUEUE_H
#include <set>
#include<vector>
#include"Timer.h"
#include"Channel.h"

namespace conex
{
class TimerQueue : noncopyable
{
public:
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();

    Timer* addTimer(TimerCallback cb, Timestamp when, NanoSecond interval);
    void cancelTimer(Timer *timer);

private:
    using Entry = std::pair<Timestamp,Timer*>;
    using TimerList = std::set<Entry>;

    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);

    EventLoop *loop_;
    const int timerfd_;
    Channel timerChannel_;
    TimerList timers_;


};
}
#endif