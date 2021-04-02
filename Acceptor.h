/**
 * @file Acceptor.h
 * @author zjb
 * @brief 
 * @date 2021-04-01
 */
#ifndef CONEX_ACCEPTOR_H
#define CONEX_ACCEPTOR_H



#include"noncopyable.h"
#include"Channel.h"
#include"InetAddress.h"
#include"Callbacks.h"

namespace conex
{
class EventLoop;

class Acceptor : noncopyable
{
public:
    Acceptor(EventLoop *loop, const InetAddress& local);
    ~Acceptor();
    
    void listen();
    bool listening() const { return listening_; }

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newCoonectionCallback_ = cb; //FIXME:这里应该也可以用std::move()
    }
private:
    void handleRead();

    EventLoop* loop_;
    const int acceptFd_;
    Channel acceptChannel_;
    InetAddress local_;
    bool listening_;
    NewConnectionCallback newCoonectionCallback_;


};
}

#endif