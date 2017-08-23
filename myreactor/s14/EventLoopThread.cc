#include "EventLoopThread.h"
#include <boost/bind.hpp>
#include "EventLoop.h"

using namespace muduo;



EventLoopThread::EventLoopThread()
    : loop_(NULL),
      exiting_(false),                            
      thread_(boost::bind(&EventLoopThread::threadFunc, this)),
      mutex_(),                  
      cond_(mutex_)
{

}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    loop_->quit();    
    thread_.join();
}


EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    {
        MutexLockGuard lock(mutex_);                
        while (NULL == loop_)            
        {
            cond_.wait();            
        }
    }

    return loop_;    
}


void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();        
    }

    loop.loop();    
}
