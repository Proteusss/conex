#include<assert.h>
#include "HttpRequest.h"

using namespace conex;
HttpRequest::HttpRequest()
    : method_(INVALID)
    , version_(UNKNOW)
{
}
bool HttpRequest::setMethod(const char* start,const char* end)
{
    assert(method_ == INVALID);
    std::string method(start,end);
    if(method == "GET")
    {
        method_ = GET;
    }
    else if(method == "POST")
    {
        method_ = POST;
    }
    else if(method == "HEAD")
    {
        method_ = HEAD;
    }
    else if(method == "PUT")
    {
        method_ = PUT;
    }
    else if(method == "DELETE")
    {
        method_ = DELETE;
    }
    else
    {
        method_ = INVALID;
    }

    return method_ != INVALID;
}
const char* HttpRequest::methodString() const
{
    std::string ret("INVALID");
    switch (method_)
    {
    case INVALID:
        ret = "INVALID";
        break;
    case GET:
        ret = "GET";
        break;
    case POST:
        ret = "POST";
        break;
    case HEAD:
        ret = "HEAD";
        break;
    case PUT:
        ret = "PUT";
        break;
    case DELETE:
        ret = "DELETE";
        break;
    default:
        break;
    }
    return ret.c_str();
}
void HttpRequest::setPath(const char* start, const char* end)
{
    path_.assign(start,end);
}
void HttpRequest::setQuery(const char* start, const char* end)
{
    query_.assign(start,end);
}
void HttpRequest::addHeader(const char* start, const char* colon, const char* end)
{
    std::string field(start, colon);
    ++colon;
    while(colon < end && isspace(*colon))
    {
        ++colon;
    }
    std::string value(colon, end);
    while(!value.empty() && isspace(value[value.size()-1]))
    {
        value.resize(value.size()-1);
    }
    headers_[field] = value;
}
std::string HttpRequest::getHeader(const std::string& field) const
{
    std::string ret;
    auto it = headers_.find(field);
    if(it != headers_.end())
    {
        ret = it->second;
    }
    return ret;
}
void HttpRequest::swap(HttpRequest& other)
{
    std::swap(method_,other.method_);
    std::swap(version_,other.version_);
    path_.swap(other.path_);
    query_.swap(other.query_);
    body_.swap(other.body_);
    std::swap(receiveTime_,other.receiveTime_);
    headers_.swap(other.headers_);
}
void HttpRequest::setBody(const char* start, const char* end)
{
    body_.assign(start,end);
}