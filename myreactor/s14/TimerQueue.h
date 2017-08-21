#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <boost/noncopyable.hpp>
#include <set>
#include <vector>
#include "Callback.h"
#include "datetime/Timestamp.h"

namespace muduo
{

class EventLoop;
class Channel;
class TimerId;

class TimerQueue : boost::noncopyable
{
    public:
        TimerQueue(EventLoop* loop);        
        ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);l


    private:
        bool insert(Timer * timer);

        void handleRead();

        typedef std::pair<Timestamp, Timer*> Entry; 
        typedef std::set<Entry> TimerList;

        EventLoop * loop_;
        const int timerfd_;
        Channel timerfdChannel_; 
        TimerList timers_;        
};

}// muduo

#endif
