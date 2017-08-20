#include "EPoller.h" 
#include "Channel.h"
#include <sys/epoll.h>
#include <errno.h>
#include "logging/Logging.h"

using namespace muduo;

namespace
{
const int kNew     = -1;
const int kAdded   = 1;
const int kDeleted = 2;
}

EPoller::EPoller(EventLoop* loop)
    : ownerLoop_(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_SYSFATAL << "EPoller::EPoller epollfd_ :" << epollfd_ << " errno :" << errno;
    }
}

EPoller::~EPoller()
{
    ::close(epollfd_);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    
    int numEvents = ::epoll_wait(epollfd_,
                                 &*events_.begin(),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);    

    Timestamp  n;
    n.now();
    if (numEvents > 0)
    {
        LOG_TRACE << numEvents << "events happended";
        fillActiveChannels(numEvents, activeChannels);                    

        if (implicit_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_TRACE << "Nothing happend...";
    }
    else
    {
        LOG_SYSERR << "EPoller::poll";
    }

    return n;
}

void EPoller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
    const int index = channel->index();
    const int fd = channel->fd();
    if (kNew == index || kDeleted == index)
    {
        if (kNew == index)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;                        
        }
        else// kDeleted == index
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } 
    else// kAdded
    {
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = static_cast<void *>(channel);
    const int fd = channel->fd();
    if(::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_SYSERR << "epoll_ctl op=" << operation << " fd = " << fd;
        }
        else
        {
            LOG_SYSFATAL << "epoll_ctl op=" << operation << " fd = " << fd;
        }
    }
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels)
{
    assert(implicit_cast<size_t>(numEvents) <= events_.size());
    for (int i=0; i<numEvents; ++i)
    {
        Channel * channel  = static_cast<Channel *>(events_[i].data.ptr);
        int fd = channel->fd();
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);                
    }
}

void EPoller::removeChannel(Channel* channel)
{
    assertInLoopThread();
    const int fd = channel->fd();
    LOG_TRACE << "fd = " << fd;
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());

    const int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void)n;
    assert(n == 1);

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    
    channel->set_index(kNew);
}
