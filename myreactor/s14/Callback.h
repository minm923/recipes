#ifndef MUDUO_NET_CALLBACK_H
#define MUDUO_NET_CALLBACK_H

#include <boost/function.hpp>

namespace muduo
{

typedef boost::function<void ()> TimerCallback;
typedef boost::function<void ()> Functor;
}

#endif //MUDUO_NET_CALLBACK_H
