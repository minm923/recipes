#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <boost/noncopyable.hpp>
#include <set>
#include <vector>
#include "Callback.h"
#include "datetime/Timestamp.h"
#include "Channel.h"

namespace muduo
{

class EventLoop;
class TimerId;
class Timer;

class TimerQueue : boost::noncopyable
{
    public:
        TimerQueue(EventLoop* loop);        
        ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);

    void cancel(TimerId timerId);

    private:
        typedef std::pair<Timestamp, Timer*> Entry; 
        typedef std::set<Entry> TimerList;

        typedef std::pair<Timer*, int64_t> ActiveTimer;
        typedef std::set<ActiveTimer> ActiveTimerSet;

        bool insert(Timer * timer);

        void handleRead();
        std::vector<Entry> getExpired(Timestamp now);
        void reset(const std::vector<Entry>& expired, Timestamp now);                

        void addTimerInLoop(Timer* timer);                

        void cancelInLoop(TimerId timerId);

        EventLoop * loop_;
        const int timerfd_;
        Channel timerfdChannel_; 
        TimerList timers_;        


        // for cancel()
        bool callingExpiredTimers_;                
        ActiveTimerSet activeTimers_;
        ActiveTimerSet cancelingExpiredTimers_;
};

}// muduo

#endif
