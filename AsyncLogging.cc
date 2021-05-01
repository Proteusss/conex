#include <assert.h>
#include <functional>
#include "AsyncLogging.h"
#include "LogFile.h"

using namespace conex;
AsyncLogging::AsyncLogging(const std::string basename, int flushInterval)
    : flushInterval_(flushInterval)
    , running_(false)
    , basename_(basename)
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
    , buffers_()
{
    assert(basename_.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}
AsyncLogging::~AsyncLogging()
{
    if(running_)
    {
        stop();
    }
}
void AsyncLogging::stop()
{
    running_ = false;
    cond_.notify_all();
    if(thread_.joinable())
        thread_.join();
}
void AsyncLogging::append(const char* logline, int len)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if(currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else
    {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if(nextBuffer_)
        {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
            currentBuffer_.reset(new Buffer);
        currentBuffer_->append(logline,len);
        cond_.notify_all();//此处存疑
    }
}
void AsyncLogging::start()
{
    running_ = true;
    thread_ = std::thread{std::bind(&AsyncLogging::threadFunc, this)};
}
void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {   //将用户写入buffer的内容移动到写线程的buffersToWrite中，
            //并把新的buffer内存移动给提供给用户的currentBuffer和nextBuffer中
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty())
            {
                cond_.wait_for(lock,std::chrono::seconds(flushInterval_));
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);//这步是干啥的？
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25) //要写的太多了
        {
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end()); //?
        }
        for(size_t i = 0; i < buffersToWrite.size(); ++i)
        {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }
        if(buffersToWrite.size() > 2)
        {
            //drop non-bzero-ed buffers, avoid
            buffersToWrite.resize(2);
        }

        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}