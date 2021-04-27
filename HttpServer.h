/**
 * @file HttpServer.h
 * @author zjb
 * @brief 
 * @date 2021-04-19
 */
#ifndef CONEX_HTTPSERVER_H
#define CONEX_HTTPSERVER_H
#include <functional>

#include "noncopyable.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpServer.h"
#include "Buffer.h"

namespace conex
{
class HttpServer : noncopyable
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse&)>;
    
    HttpServer(EventLoop* loop,const InetAddress& listenAddr);
    ~HttpServer();

    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }
    void setThreadNum(int threadNum)
    {
        server_.setThreadNum(threadNum);
    }

    EventLoop* getLoop() const
    { return server_.getLoop(); }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer& buf, Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr& conn, const HttpRequest&);
    TcpServer server_;
    HttpCallback httpCallback_;
};
}

#endif