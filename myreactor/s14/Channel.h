#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "datetime/Timestamp.h"

namespace muduo
{

class EventLoop;// 前置申明


class Channel : boost::noncopyable
{
public:
    typedef boost::function<void ()> EventCallback;
    typedef boost::function<void (Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void setReadCallback(const ReadEventCallback& cb)
    { readCallback_ = cb; }
    
    void setWriteCallback(const EventCallback& cb)
    { writeCallback_ = cb; }

    void setErrorCallback(const EventCallback& cb)
    { errorCallback_ = cb; }

    void handleEvent(Timestamp receiveTime);        

    EventLoop* ownerLoop() { return loop_; }

    int fd() { return fd_; }
    int events() { return events_; }
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    bool isWriting() { return events_ & kWriteEvent; }

    void disableAll() { events_ = kNoneEvent; update();}
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

    bool eventHandling_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};

}



#endif
