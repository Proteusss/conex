#include"ThreadPool.h"

using namespace conex;
ThreadPool::ThreadPool(size_t threadNum, size_t maxQueueSize, const ThreadInitCallback& cb)
    : maxQueueSize_(maxQueueSize)
    , running_(false)
    , threadInitCallback_(cb)
{
    for(size_t i = 1; i <= threadNum; ++i)
    {
        threads_.emplace_back(new std::thread(std::bind(&ThreadPool::runInThread,this,i)));
    }
    //LOG_INFO
}
ThreadPool::~ThreadPool()
{
    if(running_)
    {
        stop();
    }
    //LOG_INFO
}
void ThreadPool::addTask(const Task& task)
{
    if(threads_.empty())
    {
        task();
    }
    else
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(taskQueue_.size() >= maxQueueSize_)
        {
            notFull_.wait(lock);
        }
        taskQueue_.push_back(task);
        notEmpty_.notify_one();
    }
}
void ThreadPool::addTask(Task&& task)
{
    if(threads_.empty())
    {
        task();
    }
    else
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(taskQueue_.size() >= maxQueueSize_)
        {
            notFull_.wait(lock);
        }
        taskQueue_.push_back(std::move(task));
        notEmpty_.notify_one();
    }
}
void ThreadPool::stop()
{
    running_ = false;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        notEmpty_.notify_all();
    }
    for(auto& thread : threads_)
        thread->join();
}
void ThreadPool::runInThread(size_t index)
{
    if(threadInitCallback_)
        threadInitCallback_(index);
    while(running_)
    {
        if(Task task = take()) //FIXME:这里应该可以尝试使用std::move()
            task();
    }
}
Task ThreadPool::take()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while( taskQueue_.empty() && running_)
    {
        notEmpty_.wait(lock);
    }

    Task task;
    if(!taskQueue_.empty())
    {
        task = taskQueue_.front();
        taskQueue_.pop_front();
        notFull_.notify_one();
    }
    return task;
}