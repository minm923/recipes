#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <boost/noncopyable.hpp>
#include "Socket.h"
#include "Channel.h"
#include <boost/function.hpp>


namespace muduo
{

class EventLoop;    
class InetAddress;


class Acceptor : boost::noncopyable
{
public:
    typedef boost::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr);

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }

    bool listenning()
    {
        return listenning_;
    }

    void listen();


private:
    void handleRead();

    EventLoop* loop_;    
    Socket  acceptSocket_;
    Channel acceptChannel_;     
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;    
};

}// muduo

#endif
