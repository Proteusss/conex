/**
 * @file Logger.h
 * @author zjb
 * @brief 
 * @date 2021-05-01
 */
#ifndef CONEX_LOGGER_H
#define CONEX_LOGGER_H
#include <string>
#include "noncopyable.h"
#include "LogStream.h"

namespace conex
{
class Logger : noncopyable
{
public:
    Logger(const char *fileName, int line);
    ~Logger();
    LogStream& stream() 
    {
        return impl_.stream_;
    }

    static void setLogFileName(std::string filename)
    {
        logFileName_ = filename;
    }
    static std::string getLogFileName()
    {
        return logFileName_;
    }
private:
    class Impl
    {
    public:
        Impl(const char *fileName, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };

    Impl impl_;
    static std::string logFileName_;

};
#define LOG Logger(__FILE__,__LINE__).stream()
}
#endif