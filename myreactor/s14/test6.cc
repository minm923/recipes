#include "EventLoopThread.h"
#include "EventLoop.h"
#include <iostream>


void threadFunc()
{
    std::cout << "threadFunc..." << std::endl;
}


int main(int argc, char* argv[])
{
    muduo::EventLoopThread loopThread;
    muduo::EventLoop* loop = loopThread.startLoop();

    loop->runInLoop(threadFunc);
    loop->runAfter(2, threadFunc);
    sleep(3);
    loop->quit();

    return 0;    
}
