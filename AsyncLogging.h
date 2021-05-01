/**
 * @file AsyncLogging.h
 * @author zjb
 * @brief 
 * @date 2021-04-29
 */
#ifndef CONEX_ASYNCLOGGING_H
#define CONEX_ASYNCLOGGING_H
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

#include "noncopyable.h"
#include "LogStream.h"

namespace conex
{
class AsyncLogging : noncopyable
{
public:
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferPtr = std::shared_ptr<Buffer>;
    using BufferVector = std::vector<BufferPtr>;

    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging();

    void append(const char* logline, int len);
    void start();
    void stop();

private:
    void threadFunc();
    
    const int flushInterval_;
    bool running_;
    std::string basename_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};
}
#endif