#include "TcpServer.h"
#include "logging/Logging.h"
#include "Acceptor.h"
#include <boost/bind.hpp>
#include <stdio.h>// snprintf
#include "TcpConnection.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include <boost/get_pointer.hpp>

using namespace muduo;


TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : loop_(CHECK_NOTNULL(loop)),
      name_(listenAddr.toHostPort()),
      acceptor_(new Acceptor(loop_, listenAddr)),
      started_(false),
      nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(
        boost::bind(&TcpServer::newConnection, this, _1, _2));   
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    if (!started_)
    {
        started_ = true;
    }

    if (!acceptor_->listenning())
    {
        loop_->runInLoop(boost::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
}


void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();            
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;    

    LOG_INFO << "TcpServer::newConnection [" << name_
             << "] - new connection [" << connName
             << "] from " << peerAddr.toHostPort();

    InetAddress localAddress(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(
        new TcpConnection(loop_, connName, sockfd, localAddress, peerAddr));
    connections_[connName] = conn;    
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->connectEstablished();
}

