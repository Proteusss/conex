#include "HttpResponse.h"
using namespace conex;

void HttpResponse::addHeader(const std::string& key, const std::string& value)
{
    headers_[key] = value;
}
void HttpResponse::setContentType(const std::string& contentType)
{
    addHeader("Content-Type",contentType);
}
void HttpResponse::appendToBuffer(Buffer* output) const
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d", statusCode_);
    output->append(buf);
    output->append(statusMessage_);
    output->append("\r\n");
    
    if(closeConnection_)
    {   //关闭连接
        output->append("Connection: close\r\n");
    }
    else
    {
        //keep-alive需要Content-Length
        snprintf(buf, sizeof buf, "Content-Length: %zd",body_.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }
    for(auto header : headers_)
    {
        output->append(header.first);
        output->append(": ");
        output->append(header.second);
        output->append("\r\n");
    }
    output->append("\r\n");
    output->append(body_);
}
