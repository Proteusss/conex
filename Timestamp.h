/**
 * @file Timestamp.h
 * @author zjb
 * @brief 基于c++11的chrono封装时间戳类,chrono精度高，而且跨平台
 * @date 2021-03-27
 */
#ifndef CONEX_TIMESTAMP_H
#define CONEX_TIMESTAMP_H

#include<chrono>
#include<string>
#include<time.h>
namespace conex
{
using NanoSecond = std::chrono::nanoseconds;
using MicroSecond = std::chrono::microseconds;
using MilliSecond = std::chrono::milliseconds;
using Second = std::chrono::seconds;
using Minute = std::chrono::minutes;
using Hour = std::chrono::hours;

using std::chrono::system_clock;
using namespace std::literals::chrono_literals;
using Timestamp = std::chrono::time_point<system_clock,NanoSecond>;

namespace time
{
inline Timestamp now()
{
    return system_clock::now();
}
inline Timestamp nowAfter(NanoSecond interval)
{
    return now() + interval;
}
inline Timestamp nowBefore(NanoSecond interval)
{
    return now() - interval;
}
inline std::string toString(Timestamp time)
{
    std::time_t t = system_clock::to_time_t(time);
    return ctime(&t);
}
}

}// end namespace conex
#endif