#include "EventLoop.h"
#include <stdio.h>

void func1()
{
    printf("func1....\n");
}

void func2()
{
    printf("func2....\n");
}



int main(int argc, char* argv[])
{
    muduo::EventLoop loop;

    loop.runAfter(5.0, func1);
    loop.runEvery(2.0, func2);
    loop.loop();



    return 0;    
}
