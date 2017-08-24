#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include <string>
#include <boost/scoped_ptr.hpp>
#include "InetAddress.h"
#include "Callback.h"
#include <boost/enable_shared_from_this.hpp>


namespace muduo
{

class EventLoop;    

class TcpConnection : boost::noncopyable,
                      public boost::enable_shared_from_this<TcpConnection>    
{
public:

    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  const int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);

    ~TcpConnection();


    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void connectEstablished();




private:
    enum StateE { kConnecting, kConnected, };

    void setState(StateE e)
    { state_ = e; }

    void handleRead();

    EventLoop* loop_;
    std::string name_;
    StateE state_;
    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};

}

#endif
