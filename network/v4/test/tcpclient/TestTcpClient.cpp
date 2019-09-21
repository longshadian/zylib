#include <iostream>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>

#include "network/Network.h"

std::string TimeString()
{
    std::time_t t = std::time(nullptr);

    struct tm stm {};
    localtime_s(&stm, &t);
    std::array<char, 64> buf{};
    sprintf_s(buf.data(), buf.size(), "%04d-%02d-%02d %02d:%02d:%02d", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday,
        stm.tm_hour, stm.tm_min, stm.tm_sec);
    std::string s(buf.data());
    return s;
}

#define DPrintf(fmt, ...) printf("[%s] [%4d] [DEBUG  ] [%s] " fmt "\n", TimeString().c_str(), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define WPrintf(fmt, ...) printf("[%s] [%4d] [WARNING] [%s] " fmt "\n", TimeString().c_str(), __LINE__, __FUNCTION__, ##__VA_ARGS__)

std::shared_ptr<network::TcpClient> g_client = nullptr;

class TestClientEvent : public network::NetworkEvent
{
public:
    TestClientEvent() = default;
    virtual ~TestClientEvent() = default;

    virtual void OnConnect(const boost::system::error_code& ec, network::TcpConnector& connector) override
    {
        if (ec) {
            WPrintf("client connect error: %d ", ec.value());
        } else {
            DPrintf("client success ");
        }
    }

    virtual void OnAccept(const boost::system::error_code& ec, network::TcpServer& server, network::Channel& channel) override
    {
        if (ec) {
            WPrintf("error: %d ", ec.value());
        } else {
            DPrintf("success ");
        }
    }

    virtual void OnClosed(network::Channel& channel, network::ECloseType type) override
    {
        DPrintf(" closed: %d", (int)type);
    }

    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, network::Channel& channel) override
    {
        if (ec) {
            WPrintf(" code:%d length: %d ", ec.value(), (int)length);
        } else {
            DPrintf(" code:%d length: %d ", ec.value(), (int)length);
        }
    }

    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, network::Channel& channel, const network::Message& msg) override
    {
        if (ec) {
            WPrintf(" code:%d length: %d ", ec.value(), (int)length);
        } else {
            DPrintf(" code:%d length: %d ", ec.value(), (int)length);
        }
    }

    virtual void OnReceivedMessage(network::Channel& channel, std::vector<network::Message> msg_list) override
    {
        for (const auto& msg : msg_list) {
            std::string s(msg.BodyPtr(), msg.BodyPtr() + msg.BodyLength());
            DPrintf("msg:%s ", s.c_str());
        }
    }

    virtual void OnAcceptOverflow() override
    {
        WPrintf("xxx");
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

void StartClient()
{
    auto fac = std::make_shared<TestEventFactory<TestClientEvent>>();
    g_client = std::make_shared<network::TcpClient>(fac);
    g_client->Start(2);
}

int main()
{
    const std::string host = "127.0.0.1";
    std::uint16_t port = 8080;
    try {
        StartClient();

        auto conn = g_client->CreateConnector();
        if (0) {
            g_client->AsyncConnect(conn, host, port);
        } else {
            g_client->SyncConnect(conn, host, port);
        }

        int n = 0;
        while (1) {
            ++n;
            if (conn->IsConnected()) {
                conn->GetChannel()->SendMsg("aaaaaaaaaaaaaa " + std::to_string(n));
            } else {
                conn = g_client->CreateConnector();
                auto succ = g_client->SyncConnect(conn, host, port);
                if (succ) {
                    conn->GetChannel()->SendMsg("aaaaaaaaaaaaaa " + std::to_string(n));
                } else {
                    WPrintf("connect failed.");
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
        }
    } catch (const std::exception& e) {
        WPrintf("exception: %s ", e.what());
    }

    system("pause");
    return 0;
}
