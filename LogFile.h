/**
 * @file LogFile.h
 * @author zjb
 * @brief 
 * @date 2021-04-29
 */
#ifndef CONEX_LOGFILE_H
#define CONEX_LOGFILE_H

#include <string>
#include <unistd.h>
#include <mutex>
#include <memory>
#include "noncopyable.h"

namespace conex
{
class LogFile : noncopyable
{
public:
    class FileUtil : noncopyable
    {
    public:
        explicit FileUtil(std::string filename)
            : fp_(fopen(filename.c_str(),"ae")) // a:append e:O_CLOEXEC
        {
            setbuffer(fp_,buffer_,kBufferSize);
        }
        ~FileUtil()
        {
            fclose(fp_);
        }
        void append(const char* logline, const size_t len);
        void flush()
        {
            ::fflush(fp_);
        }

        static const int kBufferSize = 64 * 1024; //64K
    private:
        size_t write(const char* logline, size_t len)
        {
            //内部无锁的fwrite，因此线程不安全，但是速度更快
            return ::fwrite_unlocked(logline,1,len,fp_);
        }
        FILE* fp_;
        char buffer_[kBufferSize];
    };

    LogFile(const std::string &basename, int flushEvery = 1024);
    ~LogFile() = default;

    void append(const char* logline, int len);
    void flush();
    void rollFile();
private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<std::mutex> mutex_;
    std::unique_ptr<FileUtil> file_;
    
};
}

#endif