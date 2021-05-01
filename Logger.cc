#include <thread>
#include "Logger.h"
#include "AsyncLogging.h"
#include "Timestamp.h"
using namespace conex;
static std::once_flag once_;
static AsyncLogging *AsyncLogger_;
std::string Logger::logFileName_ = "./Conex.log";

void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}
void output(const char *logline, int len)
{
    std::call_once(once_, once_init);
    AsyncLogger_->append(logline, len);
}
Logger::Impl::Impl(const char *fileName, int line)
    : stream_()
    , line_(line)
    , basename_(fileName)
{
    formatTime();
}
void Logger::Impl::formatTime()
{
    stream_<<time::toString(time::now()).c_str();
}
Logger::Logger(const char *fileName, int line)
    : impl_(fileName, line)
{
}
Logger::~Logger()
{
    impl_.stream_<< "--" << impl_.basename_ << ":" << impl_.line_ <<"\n";
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}