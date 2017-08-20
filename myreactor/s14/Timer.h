#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H

#include <boost/noncopyable.hpp>
#include "Callback.h"

namespace muduo
{


class Timer : boost::noncopyable
{

    explicit Timer(TimerCallback& cb, Timestamp when, double interval);



    void run() const
    {
        callback_();            
    }

    Timestamp expiration() const { return expiration; }

    bool repeat() const { return repeat; }

    void restart(Timestamp now);

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        double interval_;
        bool   repeat_;
        
};

}


#endif
