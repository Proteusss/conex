/**
 * @file Callbacks.h
 * @author zjb
 * @brief 
 * @date 2021-03-30
 */
#ifndef CONEX_CALLBACKS_H
#define CONEX_CALLBACKS_H

#include<memory>
#include<functional>
#include"Timestamp.h"

namespace conex
{
class InetAddress;
class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using ConnectionCallback= std::function<void(const TcpConnectionPtr&)>;
using HighWaterMarkCall = std::function<void(const TcpConnectionPtr&,size_t)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer&, Timestamp)>;

using ErrorCallback = std::function<void()>;
using NewConnectionCallback = std::function<void(int sockfd,const InetAddress& local, const InetAddress& peer)>;
using Task = std::function<void()>;
using ThreadInitCallback = std::function<void(size_t)>;
using TimerCallback = std::function<void()>;

void defaultThreadInitCallback(size_t);
void defaultConnectionCallback(const TcpConnectionPtr&);
void defaultMessageCallback(const TcpConnectionPtr&,Buffer&,Timestamp);
}

#endif