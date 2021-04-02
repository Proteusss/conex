/**
 * @file CountDownLatch.h
 * @author zjb
 * @brief 
 * @date 2021-03-31
 */
#ifndef CONEX_COUNTDOWNLATCH_H
#define CONEX_COUNTDOWNLATCH_H

#include<mutex>
#include<condition_variable>

#include"noncopyable.h"
namespace conex
{

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count)
        : count_(count)
    {
    }

    void count()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        count_--;
        if( count_ <= 0)
            cond_.notify_all();
    }
    void wait()
    {   
        //这里为什么用unique_lock 而不是lock_guard?
        std::unique_lock<std::mutex> lock(mutex_);
        while(count_ > 0)
            cond_.wait(lock);
    }
private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

}
#endif