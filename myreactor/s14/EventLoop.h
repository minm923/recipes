#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <thread/Thread.h>
#include <vector>
#include "datetime/Timestamp.h"
#include "Callback.h"
#include "TimerId.h"

namespace muduo
{

class EPoller;    
class Channel;
class TimerQueue;

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

    Timestamp pollReturnTime() { return pollReturnTime_; }

    TimerId runAt(const Timestamp& time, const TimerCallback& cb);

    TimerId runAfter(double delay, const TimerCallback& cb);

    TimerId runEvery(double interval, const TimerCallback& cb);

private:    
    typedef std::vector<Channel*> ChannelList;
    
    void abortNotInLoopThread();

    bool looping_;    
    bool quit_;
    const pid_t threadId_;
    boost::scoped_ptr<EPoller> poller_;
    boost::scoped_ptr<TimerQueue> timerQueue_;
    ChannelList activeChannels_;
    Timestamp pollReturnTime_;
};

}

#endif
