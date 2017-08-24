#ifndef MUDUO_NET_CALLBACK_H
#define MUDUO_NET_CALLBACK_H

#include <boost/function.hpp>

namespace muduo
{
// internal use
typedef boost::function<void ()> TimerCallback;
typedef boost::function<void ()> Functor;

// client use 
class TcpConnection;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&,
                              const char* data,
                              ssize_t len)> MessageCallback;

}

#endif //MUDUO_NET_CALLBACK_H
