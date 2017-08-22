#include "Timer.h"
#include "datetime/Timestamp.h"

using namespace muduo;

void Timer::restart(Timestamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now, interval_);
    }
    else
    {
        expiration_ = Timestamp::invalid();        
    }
}
