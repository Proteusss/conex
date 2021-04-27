#include"HttpContext.h"
using namespace conex;
bool HttpContext::processRequestLine(const char* st, const char* end)
{
    bool succeed = false;
    const char* start = st;
    const char* space = std::find(start, end, ' ');

    //解析请求方法
    if( space != end && request_.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start,end,' ');
        if(space != end )
        {   
            //解析URI
            const char* question = std::find(start, space, '?');
            if(question != space)
            {
                request_.setPath(start,question);
                request_.setQuery(question,space);
            }
            else
            {
                request_.setPath(start,space);
            }
            //解析HTTP版本号
            start = space + 1;
            //共8个字节
            succeed = end - start == 8 && std::equal(start,end-1,"HTTP/1.");
            if(succeed)
            {
                if(*(end-1) == '1')
                {
                    request_.setVersion(HttpRequest::HTTP11);
                }
                else if( *(end-1) == '0')
                {
                    request_.setVersion(HttpRequest::HTTP10);
                }   
                else
                {
                    succeed = false;
                }
            }

        }
    }
    return succeed;

}
bool HttpContext::parseRequest(Buffer* buf,Timestamp receiveTime)
{
    bool ok = true, hasMore = true;
    while(hasMore)
    {   
        //解析请求行
        if(state_ == kExpectRequestLine)
        {
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                ok = processRequestLine(buf->peek(),crlf);
                if(ok) //解析成功
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf+2);
                    state_ = kExpectHeaders;
                }
            }
        }
        //解析请求头
        else if(state_ == kExpectHeaders)
        {
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                const char* colon = std::find(buf->peek(),crlf,':');
                if(colon != crlf)
                {
                    request_.addHeader(buf->peek(),colon,crlf);
                }
                else{
                    state_ = kExpectBody;
                    //state_ = kGotAll;
                    //hasMore = false;
                }
                buf->retrieveUntil(crlf+2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if(state_ == kExpectBody)
        {
            request_.setBody(buf->peek(),buf->peek()+buf->readableBytes());
            buf->retrieveAll();
            hasMore = false;
            state_=kGotAll;
        }
    }
    return ok;
}
