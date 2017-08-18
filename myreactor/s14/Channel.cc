#include <Channel.h>

using namespace muduo;

const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = POLLIN | POLLPRI | POLLRDHUP;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),    
      fd_(fd),      
      events_(0),        
      revents_(0)
{

}

void Channel::handleEvent()
{
    if (revents_ & POLLNVAL)
    {
        LOG_WARN <<"Channel::handle_event() POLLNVAL";               
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) // read
    {
        if (readCallback_) readCallback_();        
    }

    if (revents_ & (POLLOUT)) // write 
    {
        if (writeCallback_) writeCallback_();
    }

    if (revents_ & (POLLNVAL | POLLERR)) // error
    {
        if (errorCallback_) errorCallback_();
    }
}

void Channel::update()
{
    loop_->updateChannel(this);    
}
