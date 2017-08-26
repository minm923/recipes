#include "TcpConnection.h"
#include <boost/bind.hpp>
#include <assert.h>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"
#include "SocketsOps.h"

using namespace muduo;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(loop),    
      name_(name),                
      state_(kConnecting),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop_, sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr)
{
    LOG_DEBUG << "TcpConnection::ctor [ " << name_ << "] at " << this
              << " fd = " << sockfd;
    channel_->setReadCallback(
        boost::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG << "TcpConnection::dtor [ " << name_ << "] at " << this
              << " fd = " << channel_->fd();
}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();            
    assert(kConnecting == state_);
    setState(kConnected);
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof buf);

    if  (n > 0)
    {
        messageCallback_(shared_from_this(), buf, n);    
    }
    else if (0 == n)
    {
        handleClose();        
    }
    else
    {
        handleError();        
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();    
    LOG_TRACE << "TcpConnection::handleClose state = " << state_;
    assert(kConnected == state_);
    channel_->disableAll();
    closeCallback_(shared_from_this());        
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(socket_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();    
    assert(kConnected == state_);
    setState(kDisconnected);        
    channel_->disableAll();

    loop_->removeChannel(get_pointer(channel_));
}
