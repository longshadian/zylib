#include <iostream>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>

#include "network/Network.h"

#define DPrint(fmt, ...) printf("[%d] [DEBUG  ] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define WPrint(fmt, ...) printf("[%d] [WARNING] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)

class TestServerEvent : public network::NetworkEvent
{
public:
    TestServerEvent() = default;
    virtual ~TestServerEvent() = default;

    virtual void OnConnect(const boost::system::error_code& ec, network::TcpConnector& connector) override
    {

    }

    virtual void OnAccept(const boost::system::error_code& ec, network::TcpServer& server, network::Channel& channel) override
    {
        if (ec) {
            WPrint("error: %d ", ec.value());
        } else {
            DPrint("success ");
        }
    }

    virtual void OnClosed(network::Channel& channel) override
    {
        DPrint(" closed ");
    }

    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, network::Channel& channel) override
    {
        if (ec) {
            WPrint(" code:%d length: %d ", ec.value(), (int)length);
        } else {
            DPrint(" code:%d length: %d ", ec.value(), (int)length);
        }
    }

    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, network::Channel& channel, const network::Message& msg) override
    {
        if (ec) {
            WPrint(" code:%d length: %d ", ec.value(), (int)length);
        } else {
            DPrint(" code:%d length: %d ", ec.value(), (int)length);
        }
    }

    virtual void OnReceivedMessage(network::Channel& channel, std::vector<network::Message> msg_list) override
    {
        for (const auto& msg : msg_list) {
            std::string s(msg.BodyPtr(), msg.BodyPtr() + msg.BodyLength());
            DPrint(" msg:%s ", s.c_str());
        }
    }

    // handler超时
    virtual void OnTimeout(network::Channel& channel) override
    {
        WPrint(" xxx ");
    }

    // server可以得accept的handler超出上限
    virtual void OnAcceptOverflow() override
    {
        WPrint(" xxx2 ");
    }

};

class TestClientEvent : public network::NetworkEvent
{
public:
    TestClientEvent() = default;
    virtual ~TestClientEvent() = default;

    virtual void OnConnect(const boost::system::error_code& ec, network::TcpConnector& connector) override
    {
        if (ec) {
            WPrint("client connect error: %d ", ec.value());
        } else {
            DPrint("client success ");
        }
    }

    virtual void OnAccept(const boost::system::error_code& ec, network::TcpServer& server, network::Channel& channel) override
    {
        if (ec) {
            WPrint("error: %d ", ec.value());
        } else {
            DPrint("success ");
        }
    }

    virtual void OnClosed(network::Channel& channel) override
    {
        DPrint(" closed ");
    }

    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, network::Channel& channel) override
    {
        if (ec) {
            WPrint(" code:%d length: %d ", ec.value(), (int)length);
        } else {
            DPrint(" code:%d length: %d ", ec.value(), (int)length);
        }
    }

    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, network::Channel& channel, const network::Message& msg) override
    {
        if (ec) {
            WPrint(" code:%d length: %d ", ec.value(), (int)length);
        } else {
            DPrint(" code:%d length: %d ", ec.value(), (int)length);
        }
    }

    virtual void OnReceivedMessage(network::Channel& channel, std::vector<network::Message> msg_list) override
    {
        for (const auto& msg : msg_list) {
            std::string s(msg.BodyPtr(), msg.BodyPtr() + msg.BodyLength());
            DPrint(" msg:%s ", s.c_str());
        }
    }

    // handler超时
    virtual void OnTimeout(network::Channel& channel) override
    {
        WPrint(" xxx ");
    }

    // server可以得accept的handler超出上限
    virtual void OnAcceptOverflow() override
    {
        WPrint(" xxx2 ");
    }
};

template <typename T>
class TestEventFactory : public network::NetworkFactory
{
public:
    TestEventFactory() = default;
    virtual ~TestEventFactory() = default;

    virtual std::shared_ptr<network::NetworkEvent>   CreateNetworkEvent() override
    {
        return std::make_shared<T>();
    }

    virtual std::shared_ptr<network::MessageDecoder> CreateMessageDecoder() override
    {
        return network::CreateDefaultMessageDecoder();
    }
};


std::shared_ptr<network::TcpServer> g_server = nullptr;
std::shared_ptr<network::TcpClient> g_client = nullptr;

void StartServer()
{
    auto fac = std::make_shared<TestEventFactory<TestServerEvent>>();
    network::ServerOption opt{};
    g_server = std::make_shared<network::TcpServer>(fac, "0.0.0.0", 8080, opt);
    g_server->Start(5);
}

void StartClient()
{
    auto fac = std::make_shared<TestEventFactory<TestClientEvent>>();
    g_client = std::make_shared<network::TcpClient>(fac);
    g_client->Start(2);
}

int main()
{
    try {
        StartServer();
        StartClient();

        auto conn = g_client->CreateConnector();
        g_client->Connect(conn, "127.0.0.1", 8080);

        std::this_thread::sleep_for(std::chrono::seconds{ 2 });
        int n = 0;
        while (1) {
            ++n;
            conn->GetChannel()->SendMsg("aaaaaaaaaaaaaa " + std::to_string(n));
            std::this_thread::sleep_for(std::chrono::seconds{ 3 });
        }
    } catch (const std::exception& e) {
        WPrint("exception: %s ", e.what());
    }

    system("pause");

    return 0;
}
