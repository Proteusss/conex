/**
 * @file Timer.h
 * @author zjb
 * @brief 
 * @date 2021-03-30
 */
#ifndef CONEX_TIMER_H
#define CONEX_TIMER_H
#include"Channel.h"
#include"Timestamp.h"
#include"Callbacks.h"

namespace conex
{
class Timer : noncopyable
{
public:
    Timer(TimerCallback cb, Timestamp when, NanoSecond interval)
        : when_(when)
        , interval_(interval)
        , callback_( std::move(cb))
        , repeat_( interval_ > NanoSecond::zero() )
        , canceled_(false)
    {}
    bool repeat() const 
    {
        return repeat_;
    }
    bool expired(Timestamp now)
    {
        return now >= when_;
    }
    bool canceled() const
    {
        return canceled_;
    }
    Timestamp when() const
    {
        return when_;
    }
    void run()
    {
        if(callback_)
        {
            callback_();
        }
    }
    void restart()
    {
        when_ += interval_;
    }
    void cancel()
    {
        canceled_ = true;
    }
private:
    Timestamp when_;
    const NanoSecond interval_;
    bool repeat_;
    bool canceled_;

    TimerCallback callback_;
};
}
#endif