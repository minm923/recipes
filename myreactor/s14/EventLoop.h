#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <thread/Thread.h>
#include <vector>

namespace muduo
{

class EPoller;    
class Channel;

class EventLoop : boost::noncopyable 
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void assertInLoopThread()        
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() { return threadId_ == CurrentThread::tid(); }

    void updateChannel(Channel* channel);

private:    
    typedef std::vector<Channel*> ChannelList;
    
    void abortNotInLoopThread();

    bool looping_;    
    bool quit_;
    const pid_t threadId_;
    boost::scoped_ptr<EPoller> poller_;
    ChannelList activeChannels_;
};


}

#endif
