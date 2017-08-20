#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <boost/noncopyable.hpp>
#include <set>
#include <vector>



class EventLoop;
class Channel;

class TimerQueue : boost::noncopyable
{
    public:
        TimerQueue(EventLoop* loop);        
        ~TimerQueue();




    private:
        typedef std::pair<Timestamp, Timer*> Entry; 
        typedef std::set<Entry> TimerList;

        EventLoop * loop_;
        const int timerfd_;
        Channel timerfdChannel_; 
        TimerList timers_;        
};

#endif
