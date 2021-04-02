/**
 * @file EPoller.h
 * @author your name (you@domain.com)
 * @brief 
 * @date 2021-03-29
 */
#ifndef CONEX_EPOLLER_H
#define CONEX_EPOLLER_H
#include<sys/epoll.h>
#include<vector>
#include"noncopyable.h"
#include"Timestamp.h"
namespace conex
{

class EventLoop;
class Channel;

class EPoller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;
    explicit EPoller(EventLoop* loop);
    ~EPoller();
    Timestamp poll(ChannelList& channelList);
    void updateChannel(Channel* channel);

private:
    using EventList = std::vector<epoll_event>;
    static const int kInitialListSize = 16;

    void updateChannel(int op, Channel* channel);

    EventLoop *loop_;
    EventList events_;
    int epollfd_;

};
}

#endif