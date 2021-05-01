/**
 * @file LogStream.h
 * @author Lin Ya
 * @brief 
 * @date 2021-04-29
 */
#ifndef CONEX_LOGSTREAM_H
#define CONEX_LOGSTREAM_H
#include <string.h>
#include <string>
#include "noncopyable.h"
namespace conex
{
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer : noncopyable
{
public: 
    FixedBuffer()
        : cur_(data_)
    {}
    ~FixedBuffer() = default;
    void append(const char* buffer, size_t len)
    {
        if(avail() > static_cast<int>(len))
        {
            memcpy(cur_, buffer, len);
            cur_ += len;
        }
    }
    const char* data() const
    {
        return data_;
    }
    int length() const
    {
        return static_cast<int>(cur_ - data_);
    }
    char* current() const
    {
        return cur_;
    }
    int avail() const
    {
        return static_cast<int>(end() - cur_);
    }
    void add(size_t len)
    {
        cur_ += len;
    }
    void reset()
    {
        cur_ = data_;
    }
    void bzero()
    {
        memset(data_,0,sizeof(data_));
    }

private: 
    const char* end() const
    { 
        return data_ + sizeof(data_);
    }

    char data_[SIZE];
    char* cur_;
};

class LogStream : noncopyable
{
public:
    using Buffer = FixedBuffer<kSmallBuffer>;

    void append(const char* data, int len)
    {
        buffer_.append(data, len);
    }
    const Buffer& buffer() const
    {
        return buffer_;
    }
    void resetBuffer()
    {
        buffer_.reset();
    }

    // operator <<
    LogStream& operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    LogStream& operator<<(const void*);

    LogStream& operator<<(float v)
    {
        *this<<static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double);
    LogStream& operator<<(long double);
    LogStream& operator<<(char v)
    {
        buffer_.append(&v,1);
        return *this;
    }
    LogStream& operator<<(const char* str)
    {
        if(str)
            buffer_.append(str,strlen(str));
        else
            buffer_.append("(null)",6);
        return *this;
    }
    LogStream& operator<<(const unsigned char* str)
    {
        *this<<(reinterpret_cast<const char*>(str));
        return *this;
    }
    LogStream& operator<<(const std::string& v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }
private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer buffer_;
    static const int kMaxNumbericSize = 32;
};
}
#endif