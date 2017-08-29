#include "EventLoop.h"
#include "TcpServer.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include <stdio.h>
#include "Callback.h"


void connectionCallback(const muduo::TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        printf("name : %s, %s, %s\n", conn->name().c_str(),
            conn->localAddress().toHostPort().c_str(),
            conn->peerAddress().toHostPort().c_str());        
    }
    else
    {

    }
}

void messageCallback2(const muduo::TcpConnectionPtr& conn, const char* data, ssize_t len)
{
    printf("message : %s\n", data);
}

void messageCallback(const muduo::TcpConnectionPtr& conn, muduo::Buffer* data, muduo::Timestamp receiveTime)
{
    printf("message : %s\n", data->retrieveAsString().c_str());
    printf("receive time: %s\n", receiveTime.toString().c_str());
    printf("receive time: %s\n", receiveTime.toFormattedString().c_str());
}

int main(int argc, char * argv[])
{
    muduo::InetAddress listenAddress(9877);

    muduo::EventLoop loop;

    muduo::TcpServer server(&loop, listenAddress);

    server.setConnectionCallback(connectionCallback);
    server.setMessageCallback(messageCallback);
    server.start();

    loop.loop();


    return 0;    
}
