#include "EventLoop.h"
#include <thread/Thread.h>
#include <stdio.h>

muduo::EventLoop* g_oneLoop;

void ThreadFunc()
{
    printf("ThreadFunc...\n");
    g_oneLoop->loop();
}

int main(int argc, char *argv[])
{
    muduo::EventLoop oneLoop;
    g_oneLoop = &oneLoop;
    
    muduo::Thread thread(ThreadFunc);
    thread.start();
    thread.join();


    return 0;    
}
