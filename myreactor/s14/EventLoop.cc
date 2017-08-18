#include "EventLoop.h"
#include <logging/Logging.h>
#include <stdio.h>
#include "EPoller.h"
#include "Channel.h"

using namespace muduo;

__thread EventLoop* t_loopInThisThread = NULL;
const int kPollTimeMs = 10000;

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
        activeChannels_.clear();        
        poller_->poll(kPollTimeMs, &activeChannels_);        
        for (ChannelList::iterator it = activeChannels_.begin();
            it != activeChannels_.end(); ++it)
        {
            (*it)->handleEvent();
        }
    }

    LOG_TRACE << "EventLoop" << this << "stop looping";
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::updateChannel(Channel* channel)
{
    assertInLoopThread();
    assert(channel->ownerLoop() == this);
    poller_->updateChannel(channel);    
}
void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}
