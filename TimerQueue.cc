#include<sys/timerfd.h>
#include<unistd.h>
#include<ratio>
#include<strings.h>

#include"TimerQueue.h"
#include"EventLoop.h"

using namespace conex;

//匿名名称空间，仅当前文件内可用
namespace
{

int timerfdCreate()
{
    int fd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    if(fd == -1)
    {
        //LOGFATAL
    }
    return fd;
}

void timerfdRead(int fd)
{
    uint64_t val;
    ssize_t n = ::read(fd,&val,sizeof(val));
    if(n != sizeof(val))
    {
        //LOGERROR
    }
    
}
timespec durationFromNow(Timestamp when)
{
    timespec ret;
    NanoSecond ns = when - time::now();
    if(ns < 1ms)
        ns = 1ms;
    
    ret.tv_sec = static_cast<time_t>(ns.count() / std::nano::den);
    ret.tv_nsec = ns.count() % std::nano::den;
    return ret;
}
void timerfdSet(int fd, Timestamp when)
{
    itimerspec oldtime, newtime;
    bzero(&oldtime,sizeof(oldtime));
    bzero(&newtime,sizeof(newtime));
    newtime.it_value = durationFromNow(when);

    int ret = timerfd_settime(fd, 0,&newtime,&oldtime);
    if(ret == -1)
    {
        //LOG_ERROR
    }
}
}
TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop)
    , timerfd_(timerfdCreate())
    , timerChannel_(loop_, timerfd_)
{
    //timerChannel_.setReadCallback( [this](){handleRead();})
    timerChannel_.setReadCallback(std::bind(&TimerQueue::handleRead,this));
    timerChannel_.enableReading();
}
TimerQueue::~TimerQueue()
{
    for(auto& p : timers_)
        delete p.second;
    ::close(timerfd_);
}
Timer* TimerQueue::addTimer(TimerCallback cb, Timestamp when, NanoSecond interval)
{
    Timer* timer = new Timer(std::move(cb),when,interval);
    loop_->runInLoop([=](){
        auto ret = timers_.insert({when,timer});

        if(timers_.begin() == ret.first)
            timerfdSet(timerfd_,when);
    });
    return timer;
}
void TimerQueue::cancelTimer(Timer* timer)
{
    loop_->runInLoop([timer,this](){
        timer->cancel();
        timers_.erase({timer->when(),timer});
        delete timer;
    });
}
void TimerQueue::handleRead()
{
    timerfdRead(timerfd_);

    Timestamp now(time::now());
    for(auto& e: getExpired(now))
    {
        Timer* timer = e.second;

        if(!timer->canceled())
            timer->run();
        if(!timer->canceled() && timer->repeat())
        {
            timer->restart();
            e.first = timer->when();
            timers_.insert(e);
        }
        else
            delete timer;
    }

    if(!timers_.empty())
        timerfdSet(timerfd_,timers_.begin()->first);

}
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    Entry en(now + 1ns,nullptr);
    std::vector<Entry> entries;

    //这里now + 1ns的目的是什么？
    auto end = timers_.lower_bound(en);
    entries.assign(timers_.begin(),end);
    timers_.erase(timers_.begin(),end);

    return entries;
}