/**
 * @file noncopyable.h
 * @author zjb
 * @brief 不可拷贝的基类
 * @date 2021-03-27
 */
#ifndef CONEX_NONCOPYABLE_H
#define CONEX_NONCOPYABLE_H

namespace conex
{
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
} 
#endif