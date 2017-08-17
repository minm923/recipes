#define MUDUO_CHANNEL_H
#ifndef MUDUO_CHANNEL_H

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







private:

    static const int kNoneEvent;    
    static const int kReadEvent;     
    static const int kWriteEvent;



    EventLoop* loop_;            
    const int fd_;
    int events_;
    int revents_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};

}



#endif
