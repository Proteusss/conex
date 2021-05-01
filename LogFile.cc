#include "LogFile.h"

using namespace conex;

void LogFile::FileUtil::append(const char* logline, const size_t len)
{
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while( remain > 0)
    {
        size_t tmp = this->write(logline + n, remain);
        if(tmp == 0)
        {
            int err = ::ferror(fp_);
            if(err)
            {
                fprintf(stderr,"FileUtil::append() failed !\n");
            }
            break;
        }
        remain -= tmp;
    }
}
LogFile::LogFile(const std::string& basename, int flushEveryN)
    : basename_(basename)
    , flushEveryN_(flushEveryN)
    , count_(0)
    , mutex_(new std::mutex)
    , file_(new FileUtil(basename))
{
}
void LogFile::append(const char* logline, int len)
{
    std::lock_guard<std::mutex> guard(*mutex_);
    append_unlocked(logline,len); 
}
void LogFile::flush()
{
    std::lock_guard<std::mutex> guard(*mutex_);
    file_->flush();
}
void LogFile::append_unlocked(const char* logline, int len)
{
    file_->append(logline,len);
    ++count_;
    if(count_ >= flushEveryN_)
    {
        count_ = 0;
        file_->flush();
    }
}