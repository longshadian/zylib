#pragma once

#include <iostream>

#include <net/TcpServer.h>
#include <net/InetAddress.h>
#include <net/console_log.h>
#include <net/EventLoop.h>
#include <net/Buffer.h>
#include <net/TcpConnection.h>
#include <net/Utility.h>

void Server_ConnectionCB(zysoft::net::TcpConnectionPtr conn)
{
    NET_LOG_INFO << "ConnectionCB new conn: " << conn->GetConnName();
}

void Server_MessageCB(zysoft::net::TcpConnectionPtr conn, zysoft::net::Buffer* buffer)
{
    //std::string s(buffer->ReadablePtr(), buffer->ReadablePtr() + buffer->ReadableBytes());
    NET_LOG_INFO << "MessageCB size: " << buffer->ReadableBytes();
    //buffer->Clear();
    const char* p = zysoft::net::util::FindCRLF(*buffer);
    if (!p) {
        return;
    }
    std::string s2(buffer->ReadablePtr(), p);
    buffer->Consume(p + 2 - buffer->ReadablePtr());
    conn->Send(s2 + "<<\r\n");
    if (s2 == "shutdown") {
        NET_LOG_INFO << "------------------------> shutdown " << conn->GetConnName();
        for (int i = 0; i != 10; ++i) {
            std::string s = "shutdown ====================> " + std::to_string(i) + "<<\r\n";
            conn->Send(s);
        }
        conn->Shutdown(true);
        conn->ForceClose();
    }
    NET_LOG_INFO << "MessageCB " << s2 << " size: " << buffer->ReadableBytes();
}

void TestEcho()
{
    try {
        std::string ip = "0.0.0.0";
        std::uint16_t port = 10086;
        zysoft::net::InetAddress addr{ip, port};
        zysoft::net::EventLoop loop;
        zysoft::net::TcpServer server{&loop, addr};
        server.SetReuseAddr(true);
        //server.SetConnectionCallback(std::bind(&Server_ConnectionCB, std::placeholders::_1));
        server.SetMessageCallback(std::bind(&Server_MessageCB, std::placeholders::_1, std::placeholders::_2));

        server.Start();
        loop.Loop();
    } catch (const std::exception& e) {
        NET_LOG_CRIT << "TestEcho exception: " << e.what();
    }
}

int main()
{
    TestEcho();
    return 0;
}

