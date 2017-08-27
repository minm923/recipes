#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"
#include <sys/epoll.h>
#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = POLLIN | POLLPRI | POLLRDHUP;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      eventHandling_(false)
{

}

Channel::~Channel()
{
    assert(!eventHandling_);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    eventHandling_ = true;

    if (revents_ & POLLNVAL)
    {
        LOG_WARN <<"Channel::handle_event() POLLNVAL";               
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) // read
    {
        if (readCallback_) readCallback_(receiveTime);        
    }

    if (revents_ & (POLLOUT)) // write 
    {
        if (writeCallback_) writeCallback_();
    }

    if (revents_ & (POLLNVAL | POLLERR)) // error
    {
        if (errorCallback_) errorCallback_();
    }

    eventHandling_ = false;
}

void Channel::update()
{
    loop_->updateChannel(this);    
}
