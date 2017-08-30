#include "TcpConnection.h"
#include <boost/bind.hpp>
#include <assert.h>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"
#include "SocketsOps.h"
#include "Buffer.h"

#include <errno.h>

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
        boost::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(
        boost::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(
        boost::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(
        boost::bind(&TcpConnection::handleError, this));
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

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if  (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);    
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
    assert(kConnected == state_ || kDisconnecting == state_);
    channel_->disableAll();
    closeCallback_(shared_from_this());        
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(socket_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::handleWrite()
{
    loop_->assertInLoopThread();
    if (channel_->isWriting())
    {
        ssize_t n = ::write(channel_->fd(),
                            outputBuffer_.peek(),
                            outputBuffer_.readableBytes());
        if (n >= 0)
        {
            outputBuffer_.retrieve(n);
            if (0 == outputBuffer_.readableBytes())
            {
                channel_->disableWriting();
                if (kDisconnecting == state_)
                {
                    shutdownInLoop();                    
                }
            }
            else
            {
                LOG_TRACE << "I am going to write more data";
            }
        }
        else
        {
            LOG_SYSERR << "TcpConnection::handleWrite";
        }
    }
    else
    {
        LOG_TRACE << "TcpConnection is down, no more writing";
    }
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();    
    assert(kConnected == state_ || kDisconnecting == state_ );
    setState(kDisconnected);        
    channel_->disableAll();
    connectionCallback_(shared_from_this());

    loop_->removeChannel(get_pointer(channel_));
}

void TcpConnection::shutdown()
{
    if (kConnected == state_)
    {
        setState(kDisconnecting);
        loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::setTcpNodelay(bool on)
{
    socket_->setTcpNoDelay(on);
}

void TcpConnection::setKeepAlive(bool on)
{
    socket_->setKeepAlive(on);
}

void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if (!channel_->isWriting())
    {
        socket_->shutdownWrite();// 设置了状态 在handleWrite()的之后判断状态 来关闭
    }
}

void TcpConnection::send(const std::string& message)
{
    if (kConnected == state_)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            loop_->runInLoop(
                boost::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    loop_->assertInLoopThread();
    ssize_t nwrite = 0;

    if (!channel_->isWriting() && 0 == outputBuffer_.readableBytes())
    {
        nwrite = ::write(channel_->fd(), message.data(), message.size());
        if (nwrite >= 0)
        {
            if (implicit_cast<size_t>(nwrite) < message.size())
            {
                LOG_DEBUG << "I am going to write more data..." << (message.size()-nwrite);
            }
        }
        else
        {
            nwrite = 0;
            if (errno != EWOULDBLOCK)// 对于非阻塞socket EAGIN和EWOULDBLOCK不是错误
            {
                LOG_SYSERR << "TcpConnection::sendInLoop";
            }
        }
    }

    assert(nwrite >= 0);

    if (implicit_cast<size_t>(nwrite) < message.size())
    {
        outputBuffer_.append(message.data()+nwrite, message.size()-nwrite);        
        if (!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}
