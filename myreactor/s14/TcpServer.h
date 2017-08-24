#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include <boost/noncopyable.hpp>
#include <string>
#include <boost/scoped_ptr.hpp>
#include "Callback.h"
#include <map>
#include "InetAddress.h"


namespace muduo
{

class EventLoop;    
class Acceptor;

class TcpServer : boost::noncopyable
{
public:

    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();// force out-line dtor, for scoped_ptr members;    

    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)    
    { messageCallback_ = cb; }


private:
    void newConnection(int sockfd, const InetAddress& peerAddr);        

    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;    
    const std::string name_;    
    boost::scoped_ptr<Acceptor> acceptor_;        
    ConnectionCallback connectionCallback_;    
    MessageCallback messageCallback_;            
    bool started_;
    int nextConnId_;
    ConnectionMap connections_;
};

}



#endif
