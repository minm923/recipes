#include "Timer.h"

using namespace muduo;

void Timer::restart(Timerstamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now, interval);
    }
    else
    {
        expiration_ = Timerstamp::invalid();        
    }
}
