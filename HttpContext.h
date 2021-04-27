/**
 * @file HttpContext.h
 * @author zjb
 * @brief 
 * @date 2021-04-14
 */
#ifndef CONEX_HTTPCONTEXT_H
#define CONEX_HTTPCONTEXT_H
#include "Buffer.h"
#include "HttpRequest.h"
namespace conex
{

class HttpContext
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };

    HttpContext()
        : state_(kExpectRequestLine)
    {
    }
    bool getAll() const
    { return state_ == kGotAll; }
    bool parseRequest(Buffer* buf, Timestamp receiveTime);
    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest request;
        request_.swap(request);
    }
    const HttpRequest& request() const
    { return request_; }
    HttpRequest& request() 
    { return request_; }


private:
    bool processRequestLine(const char* start, const char* end);
    HttpRequest request_;
    HttpRequestParseState state_;
};

}
#endif