#include <EventLoop.h>
#include "TimerQueue.h"
#include "Timer.h"
#include <sys/timerfd.h>
#include "TimerId.h"

namespace muduo
{

namespace detail
{

int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    
    if (timerfd < 0)
    {
        LOG_SYSFATAL << "Fail in timerfd_create";
    }

    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch()
                            - Timestamp::now().microSecondsSinceEpoch();

    if (microseconds < 100)
    {
        microseconds = 100;        
    }
    
    struct timespec ts;
    ts.tv_sec  = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(microseconds % Timestamp::kMicroSecondsPerSecond * 1000);

    return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_SYSERR << "timerfd_settime()";
    }
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE << "TimerQueue::handleRead()" << howmany << " at" << now.toString();
    if (n != sizeof howmany)
    {
        LOG_ERR << "TimerQueue::handleRead() reads" << n << " bytes instead of 8";
    }
}

}// detail
}// muduo



using namespace muduo;
using namespace muduo::detail;

TimerQueue::TimerQueue(EventLoop * loop)
    : loop_(loop),    
      timerfd_(createTimerfd()),
      timerfdChannel_(loop, timerfd_),
      timers_()
{
    timerfdChannel_.setReadCallback();
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    ::close(timerfd_);

    for (TimerList::iterator it  = timerfd_.begin();
        it != timerfd_.end();++it)    
    {
        delete it->second;                
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    Timer * timer = new Timer(cb, when, interval);
    loop_->assertInLoopThread();    
    bool earlistChanged = insert(timer);

    if (earlistChanged)
    {
        resetTimerfd(timerfd_, timer->expiration());
    }
    
    return TimerId(timer);
}

bool TimerQueue::insert(Timer * timer)
{
    bool earlistChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (timers_.end() == it || when < it->first)
    {
        earlistChanged = true;        
    }

    std::pair<TimerList::iterator, bool> result = 
        timers_.insert(std::make_pair<when, timer>);

    assert(result->second);    
    return earlistChanged;
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());

    readTimerfd(timerfd_, now);

    // 获得到期定时器
    std::vector<Entry> expired = getExpired(now);

    // 回调
    for (std::vector<Entry>::iterator it = expired.begin();
        it != expired.end(); ++it)
    {
        it->second->run();
    }
    
    // 重置定时器
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, back_inserter(expired));
    timers_.erase(timers_.begin(), it);
    
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    for (std::vector<Entry>::iterator it = expired.begin();
        it != expired.end(); ++it)
    {
        if (it->second->repeat())
        {
            it->second->restart(now);
            insert(it->second);
        }
        else
        {
            delete it->second;            
        }
    }
    
    Timestamp nextExpire;     
    if (!timers_.empty())
    {
        nextExpire = timers_.begin()->first;
    }

    if (nextExpire.valid())
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}
