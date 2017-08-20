#include <iostream>
#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>

muduo::EventLoop* g_loop = NULL;


void timerout()
{
    std::cout << "time out...";
    g_loop->quit();
}

int main(int argc, char * argv[])
{
    muduo::EventLoop loop;
    g_loop = &loop;


    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    muduo::Channel channel(g_loop, timerfd); 
    channel.setReadCallback(timerout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();


    ::close(timerfd);




    return 0;    
}
