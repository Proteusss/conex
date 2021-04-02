#include<sys/epoll.h>
#include<cassert>

#include"Channel.h"
#include"EventLoop.h"

using namespace conex;
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : pooling_(false)
    , loop_(loop)
    , fd_(fd)
    , tied_(false)
    , events_(0)
    , revents_(0)
    , handlingEvents_(false)
{
}
Channel::~Channel()
{
    assert(!handlingEvents_);
}
void Channel::handleEvents(Timestamp receiveTime)
{
    if(tied_)
    {
        auto guard = tie_.lock();
        if(guard != nullptr)
            handleEventWithGuard(receiveTime);
    }
}
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    //LOG_INFO here
    handlingEvents_ = true;
    if( (revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if( closeCallback_ )
            closeCallback_();
    }
    if( revents_ & EPOLLERR )
    {
        if( errorCallback_ )
            errorCallback_();
    }
    if( revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        if(readCallback_)
            readCallback_(receiveTime);
    }
    if( revents_ & EPOLLOUT )
    {
        if(writeCallback_)
            writeCallback_();
    }
    handlingEvents_ = false;
}
//这个函数会在何处被调用呢？
void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}
void Channel::update()
{
   loop_->updateChannel(this);
}
void Channel::remove()
{
    loop_->removeChannel(this);
}