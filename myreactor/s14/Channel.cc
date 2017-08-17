#include <Channel.h>

using namespace muduo;

const int Channel::kNoneEvent = ;
const int Channel::kReadEvent = 

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),    
      fd_(fd),      
      events_(0),        
      revents_(0)
{

}




























