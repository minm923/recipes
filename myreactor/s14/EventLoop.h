#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <boost/noncopyable.h>

namespace muduo
{

class EventLoop : boost::noncopyable 
{
Public:
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

    bool isInLoopThread() { return threadId_ == CurrentThread::Tid() }

private:    
    void abortNotInLoopThread()

    bool looping_;    
    const pid_t threadId_;
};


}





















#endif
