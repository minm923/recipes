#ifndef MUDUO_NET_EVENTLOOPPTHREAD_H
#define MUDUO_NET_EVENTLOOPPTHREAD_H

#include <boost/noncopyable.hpp>
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"



namespace muduo
{

class EventLoop;    

class EventLoopThread : boost::noncopyable
{
public:    
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();


private:
    void threadFunc();        

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
};


}

#endif
