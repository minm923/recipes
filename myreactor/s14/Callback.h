#ifndef MUDUO_NET_CALLBACK_H
#define MUDUO_NET_CALLBACK_H

#include <boost/function.hpp>
#include "datetime/Timestamp.h"

namespace muduo
{
// internal use
typedef boost::function<void ()> TimerCallback;
typedef boost::function<void ()> Functor;

// client use 
class Buffer;
class TcpConnection;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              Timestamp)> MessageCallback;

typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;


}

#endif //MUDUO_NET_CALLBACK_H
