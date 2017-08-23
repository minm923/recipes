#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <stdio.h>
#include "SocketsOps.h"

void newconnection(int fd, const muduo::InetAddress& peerAddr)
{
    printf("new connection from  %s",peerAddr.toHostPort().c_str());
    ::write(fd, "how r u?\n", 9);        
    muduo::sockets::close(fd);
}

int main(int argc, char* argv[])
{
    muduo::EventLoop loop;
    muduo::InetAddress listenaddr(9877);
    muduo::Acceptor  acceptor(&loop, listenaddr);
    acceptor.setNewConnectionCallback(newconnection);
    acceptor.listen();

    loop.loop();

    return 0;
}
