/**
 * @file ThreadPool.h
 * @author zjb
 * @brief 
 * @date 2021-03-31
 */
#ifndef CONEX_THREADPOOL_H
#define CONEX_THREADPOOL_H
#include<thread>
#include<vector>
#include<mutex>
#include<deque>
#include<atomic>
#include<condition_variable>

#include"noncopyable.h"
#include"Callbacks.h"
namespace conex
{

class ThreadPool : noncopyable
{
public:
    explicit ThreadPool(size_t threadNum, size_t maxQueueSize = 65536, const ThreadInitCallback& cb = nullptr );
    ~ThreadPool();
    size_t threadNum() const
    {
        return threads_.size();
    }
    void addTask(const Task& task);
    void addTask(Task&& task);
    void stop();
private:
    void runInThread(size_t index);
    Task take();

    using ThreadPtr = std::unique_ptr<std::thread>;
    using ThreadList = std::vector<ThreadPtr>;

    ThreadList threads_;
    std::mutex mutex_;
    std::atomic_bool running_;
    
    const size_t maxQueueSize_;
    std::deque<Task> taskQueue_;

    std::condition_variable notEmpty_;
    std::condition_variable notFull_;

    ThreadInitCallback threadInitCallback_;
    
};

}
#endif