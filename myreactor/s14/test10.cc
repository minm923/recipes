#include "EventLoop.h"
#include "TcpServer.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include <stdio.h>
#include "Callback.h"
#include <string>

std::string message1;
std::string message2;

void connectionCallback(const muduo::TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        printf("name : %s, %s, %s\n", conn->name().c_str(),
            conn->localAddress().toHostPort().c_str(),
            conn->peerAddress().toHostPort().c_str());        
        conn->send(message1);
        conn->send(message2);
        conn->shutdown();
    }
    else
    {
        printf("connection %s is down...\n", conn->name().c_str());
    }
}

void messageCallback2(const muduo::TcpConnectionPtr& conn, const char* data, ssize_t len)
{
    printf("message : %s\n", data);
}

void messageCallback(const muduo::TcpConnectionPtr& conn, muduo::Buffer* data, muduo::Timestamp receiveTime)
{
    std::string message(data->retrieveAsString());
    printf("message : %s\n", message.c_str());
    printf("receive time: %s\n", receiveTime.toString().c_str());
    printf("receive time: %s\n", receiveTime.toFormattedString().c_str());

    conn->send(message);
}

int main(int argc, char * argv[])
{
    int len1 = 100;
    int len2 = 200;
    if (argc > 2)
    {
        len1 = atoi(argv[1]);
        len2 = atoi(argv[2]);
    }

    message1.resize(len1);
    message2.resize(len2);

    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    muduo::InetAddress listenAddress(9877);

    muduo::EventLoop loop;

    muduo::TcpServer server(&loop, listenAddress);

    server.setConnectionCallback(connectionCallback);
    server.setMessageCallback(messageCallback);
    server.start();

    loop.loop();


    return 0;    
}
