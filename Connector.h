/**
 * @file Connector.h
 * @author zjb
 * @brief 
 * @date 2021-04-02
 */
#ifndef CONEX_CONNECTOR_H
#define CONEX_CONNECTOR_H

#include"noncopyable.h"
#include"InetAddress.h"
#include"Channel.h"
#include"Callbacks.h"
namespace conex
{
class EventLoop;

class Connector : noncopyable
{
public:
    Connector(EventLoop *loop,const InetAddress& peer);
    ~Connector();

    void start();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb; //FIXME:应该可以用std::move
    }
    void setErrorCallback(const ErrorCallback& cb)
    {
        errorCallback_ = cb;
    }
private:
    void handleWrite();

    EventLoop *loop_;
    const int sockfd_;
    Channel channel_;    
    const InetAddress peer_;

    bool connected_;
    bool started_;
    
    NewConnectionCallback newConnectionCallback_;
    ErrorCallback errorCallback_;

};
}
#endif