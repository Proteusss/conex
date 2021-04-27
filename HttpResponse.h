/**
 * @file HttpResponse.h
 * @author zjb
 * @brief 
 * @date 2021-04-14
 */
#ifndef CONEX_HTTPRESPONSE_H
#define CONEX_HTTPRESPONSE_H
#include <string>
#include <map>

#include "Buffer.h"
namespace conex
{
class HttpResponse
{
public:
    enum HttpStatusCode
    {
        CODE_UNKNOWN,
        CODE_200 = 200,
        CODE_301 = 301,
        CODE_400 = 400,
        CODE_404 = 404,
        CODE_501 = 501
    };
    explicit HttpResponse(bool close)
        : statusCode_(CODE_UNKNOWN)
        , closeConnection_(close)
        , cgi_(0)
    {
    }
    void setStatusCode(HttpStatusCode code)
    { statusCode_ = code; }
    void setStatusMessage(const std::string& msg)
    { statusMessage_ = msg;}
    void setCloseConnection(bool on)
    { closeConnection_ = on; }
    bool closeConnection() const
    { return closeConnection_; }

    void addHeader(const std::string& key, const std::string& value);
    void setContentType(const std::string& contentType);
    void setBody(const std::string& body)
    { body_ = body; }
    void appendToBuffer(Buffer* output) const;
    
    // void setCgi(int c);
    // int cgi() const;

private:
    //状态码和状态信息
    HttpStatusCode statusCode_;
    std::string statusMessage_;
    //响应头
    std::map<std::string,std::string> headers_;
    std::string body_;
    bool closeConnection_;

    int cgi_;

};
}
#endif