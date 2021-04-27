/**
 * @file EPoller.cc
 * @author your name (you@domain.com)
 * @brief 
 * @date 2021-03-29
 */
#include<unistd.h>

#include"EPoller.h"
#include"EventLoop.h"
#include"Channel.h"

using namespace conex;

EPoller::EPoller(EventLoop* loop)
    : loop_(loop)
    , epollfd_(epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitialListSize)
{
    if(epollfd_ < 0)
    {
        //LOG_FATAL
    }
}
EPoller::~EPoller()
{
    ::close(epollfd_);
}   
void EPoller::updateChannel(Channel* channel)
{
    int op = 0;
    if(!channel->pooling())
    {
        op = EPOLL_CTL_ADD;
        channel->set_pooling(true);
    }
    else if(!channel->isNoneEvent())
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        op = EPOLL_CTL_DEL;
        channel->set_pooling(false);
    }
    updateChannel(op,channel);

}
void EPoller::updateChannel(int op, Channel* channel)
{
    epoll_event event;
    event.events = channel->events();
    event.data.ptr = channel; // 注意这里直接把channel绑给epoll_event
    int ret = ::epoll_ctl(epollfd_,op,channel->fd(),&event);
    if(ret < 0)
    {
        //LOG_ERROR
    }

}
Timestamp EPoller::poll(ChannelList& activeChannel)
{
    int maxEvents = static_cast<int>(events_.size());
    int numEvents = ::epoll_wait(epollfd_,events_.data(),maxEvents,1000);//注意这里把epollwait设置成阻塞的了
    Timestamp now = time::now();
    if(numEvents == -1)
    {
        if(errno != EINTR)
        {
            //LOGERR
        }
    }
    else if(numEvents > 0)
    {
        for( int i = 0; i < numEvents; ++i)
        {
            auto channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->set_revents(events_[i].events);
            activeChannel.push_back(channel);
        }
        if(numEvents == maxEvents)
        {
            events_.resize( 2 * events_.size());
        }
    }
    return now;
}