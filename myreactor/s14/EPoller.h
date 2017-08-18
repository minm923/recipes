#ifndef MUDUO_NET_EPOLLER_H
#define MUDUO_NET_EPOLLER_H

#include <vector>
#include <map>

#include "EventLoop.h"
#include <datetime/Timestamp.h>

struct epoll_event;

namespace muduo
{

class Channel;

class EPoller : boost::noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    EPoller(EventLoop* loop);            
    ~EPoller();

    void updateChannel(Channel* channel);        
    
    void removeChannel(Channel* channel);

    Timestamp poll(int timeoutMs, ChannelList* activeChannels);

    void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }


private:
    void update(int operation, Channel* channel);

    void fillActiveChannels(int numEvents, ChannelList* activeChannels);

    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int, Channel*> ChannelMap;

    static const int kInitEventListSize = 16;

    EventLoop * ownerLoop_;
    int epollfd_;
    EventList events_;
    ChannelMap channels_;
};

}

#endif
