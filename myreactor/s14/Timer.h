#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H

#include <boost/noncopyable.hpp>
#include "Callback.h"
#include "datetime/Timestamp.h"
#include "thread/Atomic.h"

namespace muduo
{


class Timer : boost::noncopyable
{
public:
    explicit Timer(const TimerCallback& cb, Timestamp when, double interval)
        : callback_(cb),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequence_(s_numCreated_.incrementAndGet())
    { }

    void run() const
    {
        callback_();            
    }

    Timestamp expiration() const { return expiration_; }

    bool repeat() const { return repeat_; }

    void restart(Timestamp now);

    int64_t sequence() const { return sequence_; }

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        double interval_;
        bool   repeat_;
        const int64_t sequence_;

        static AtomicInt64 s_numCreated_;
};

}// muduo
#endif
