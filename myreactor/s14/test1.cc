#include "EventLoop.h"


int main(int argc, char *argv[])
{

    muduo::EventLoop oneLoop;
    oneLoop.loop();

    return 0;    
}
