/**
 * @file HttpRequest.h
 * @author zjb
 * @brief 
 * @date 2021-04-14
 */
#ifndef CONEX_HTTPREQUEST_H
#define CONEX_HTTPREQUEST_H
#include<string>
#include<map>
#include "Timestamp.h"
namespace conex
{
class HttpRequest
{
public:
    enum Method
    {
        INVALID ,
        GET ,
        POST ,
        HEAD ,
        PUT ,
        DELETE
    };
    enum Version
    {
        UNKNOW ,
        HTTP10 ,
        HTTP11
    };

    HttpRequest();
    bool setMethod(const char* start,const char* end);
    Method method() const
    {return method_;}
    const char* methodString() const;

    void setVersion(Version v)
    { version_ = v; }
    Version getVersion() const 
    { return version_; }

    void setPath(const char* start, const char* end);
    const std::string& path() const
    { return path_; }

    void setQuery(const char* start, const char* end);
    const std::string& query() const
    { return query_; }

    void setReceiveTime(Timestamp time)
    { receiveTime_ = time; }
    Timestamp receiveTime() const
    { return receiveTime_; } 

    void addHeader(const char* start, const char* colon, const char* end);
    std::string getHeader(const std::string& field) const;
    const std::map<std::string,std::string>& headers() const
    { return headers_; }

    void swap(HttpRequest& other);
    void setBody(const char* start, const char* end);
    std::string getBody() const
    { return body_; }


private:
    Method method_;
    std::string path_;
    Version version_;
    std::string query_;
    std::string body_;
    Timestamp receiveTime_;
    std::map<std::string,std::string> headers_;
};
}

#endif