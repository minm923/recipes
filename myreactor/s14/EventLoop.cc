#include "EventLoop.h"
#include <logging/Logging.h>
#include <stdio.h>

using namespace muduo;

__thread EventLoop* t_loopInThisThread = NULL;

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      threadId_(CurrentThread::tid()),
      poller_(new EPoller(this))  
{
    LOG_TRACE << "EventLoop created " << this  << " in thread " << threadId_;
    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop" << t_loopInThisThread << "in thread" << threadId_;
    }
    else
    {
        t_loopInThisThread = this;    
    }
}

EventLoop::~EventLoop()
{
    t_loopInThisThread = NULL;            
}

void EventLoop::loop()
{
    assertInLoopThread();
    assert(!looping_);            
    looping_ = true;    
    quit_    = false;        

    while(!quit_)
    {

    }

    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}
