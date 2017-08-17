#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <thread/Thread.h>

namespace muduo
{

class EventLoop : boost::noncopyable 
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread()        
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() { return threadId_ == CurrentThread::tid(); }

private:    
    void abortNotInLoopThread();

    bool looping_;    
    const pid_t threadId_;
};


}





















#endif
