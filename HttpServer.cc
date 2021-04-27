#include "HttpServer.h"
#include "TcpConnection.h"
using namespace conex;

void defaultHttpCallback(const HttpRequest& req, HttpResponse& resp)
{
    resp.setStatusCode(HttpResponse::CODE_404);
    resp.setStatusMessage("Not Found");
    resp.setCloseConnection(true);
}
HttpServer::HttpServer(EventLoop* loop,const InetAddress& listenAddr)
    : server_(loop, listenAddr)
    , httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection,this,std::placeholders::_1)
    );
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)
    );
}
HttpServer::~HttpServer()
{
}
void HttpServer::start()
{
    //LOG_INFO
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setContext(HttpContext());
    }
}
void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer& buf, Timestamp receiveTime)
{
    HttpContext* context = any_cast<HttpContext>(&(conn->getContext())); //FIXME: may bug in here
    if(!context->parseRequest(&buf,receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if(context->getAll())
    {
        onRequest(conn,context->request());
        context->reset();
    }
}
void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& request)
{
    std::string connection = request.getHeader("Connection");
    bool close = ( connection == "close" ||
                request.getVersion() == HttpRequest::HTTP10  && connection != "Keep-Alive");
    HttpResponse response(close);
    //用户设置回调对response进行设置
    httpCallback_(request,response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(buf);
    //非keep-alive情况下则关闭连接
    if(close)
    {
        conn->shutdown();
    }
}