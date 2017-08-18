#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace muduo
{

class EventLoop;// 前置申明


class Channel : boost::noncopyable
{
public:
    typedef boost::function<void ()> EventCallback;

    Channel(EventLoop* loop, int fd);

    void setReadCallback(const EventCallback& cb)
    { readCallback_ = cb; }
    
    void setWriteCallback(const EventCallback& cb)
    { writeCallback_ = cb; }

    void setErrorCallback(const EventCallback& cb)
    { errorCallback_ = cb; }

    void handleEvent();        

    EventLoop* ownerLoop() { return loop_; }

    int fd() { return fd_; }
    int events() { return events_; }
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent; update(); }
private:

    void update();

    static const int kNoneEvent;    
    static const int kReadEvent;     
    static const int kWriteEvent;

    EventLoop* loop_;            
    const int fd_;
    int events_;
    int revents_;
    int index_;// used by EPoller

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};

}



#endif
