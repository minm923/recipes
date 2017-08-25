#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include <string>
#include <boost/scoped_ptr.hpp>
#include "InetAddress.h"
#include "Callback.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>


namespace muduo
{

class EventLoop;    
class Socket;
class Channel;

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

    // Internal use only
    void setCloseCallback(const CloseCallback& cb)    
    { closeCallback_ = cb; }
    
    void connectEstablished();

    // called when TcpServer has removed me from its map
    void connectDestroyed();

    const std::string& name() const { return name_; }

    bool connected() const { return state_ == kConnected ;}

    const InetAddress& localAddress() { return localAddr_;}

    const InetAddress& peerAddress() { return peerAddr_;}

private:
    enum StateE { kConnecting, kConnected, };

    void setState(StateE e)
    { state_ = e; }

    void handleRead();

    void handleWrite();

    void handleClose();
    
    void handleError();

    EventLoop* loop_;
    std::string name_;
    StateE state_;
    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback   closeCallback_;
};

}

#endif
