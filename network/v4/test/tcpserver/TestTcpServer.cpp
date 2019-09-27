#include <iostream>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>
#include <atomic>

#include "network/Network.h"

std::string TimeString()
{
    std::time_t t = std::time(nullptr);

    struct tm stm{};
    localtime_s(&stm, &t);
    std::array<char, 64> buf{};
    sprintf_s(buf.data(), buf.size(), "%04d-%02d-%02d %02d:%02d:%02d", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday,
        stm.tm_hour, stm.tm_min, stm.tm_sec);
    std::string s(buf.data());
    return s;
}

#define DPrintf(fmt, ...) printf("[%s] [%4d] [DEBUG  ] [%s] " fmt "\n", TimeString().c_str(), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define WPrintf(fmt, ...) printf("[%s] [%4d] [WARNING] [%s] " fmt "\n", TimeString().c_str(), __LINE__, __FUNCTION__, ##__VA_ARGS__)

struct Counter
{
    void AddCount(const std::thread::id& index)
    {
        std::ostringstream ostm{};
        ostm << index;
        ++m_cnt[ostm.str()];
    }

    std::string ToString() const
    {
        std::ostringstream ostm{};
        for (const auto& it : m_cnt) {
            ostm << it.first  << ":" << it.second.load() << " ";
        }
        return ostm.str();
    }

    std::unordered_map<std::string, std::atomic<int32_t>> m_cnt;
};

std::shared_ptr<network::TcpServer> g_server = nullptr;
Counter g_count{};

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
            WPrintf("error: %d %s", ec.value(), ec.message().c_str());
        } else {
            DPrintf("success ");
        }
    }

    virtual void OnClosed(network::Channel& channel, network::ECloseType type) override
    {
        DPrintf("closed type:%d", (int)type);
    }

    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, network::Channel& channel) override
    {
        return;
        if (ec) {
            WPrintf(" code:%d length: %d %s", ec.value(), (int)length, ec.message().c_str());
        } else {
            DPrintf(" code:%d length: %d %s", ec.value(), (int)length, ec.message().c_str());
        }
    }

    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, network::Channel& channel, const network::Message& msg) override
    {
        return;
        if (ec) {
            WPrintf(" code:%d length: %d %s", ec.value(), (int)length, ec.message().c_str());
        } else {
            DPrintf(" code:%d length: %d %s", ec.value(), (int)length, ec.message().c_str());
        }
    }

    virtual void OnReceivedMessage(network::Channel& channel, std::vector<network::Message> msg_list) override
    {
        for (const auto& msg : msg_list) {
            std::string s(msg.BodyPtr(), msg.BodyPtr() + msg.BodyLength());
            DPrintf(" msg:%s ", s.c_str());
            g_count.AddCount(std::this_thread::get_id());
        }
    }

    virtual void OnAcceptOverflow() override
    {
        WPrintf(" xxx2 ");
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

void StartServer()
{
    auto fac = std::make_shared<TestEventFactory<TestServerEvent>>();
    network::ServerOption opt{};
    opt.m_read_timeout_seconds = 10;
    opt.m_write_timeout_seconds = 10;
    g_server = std::make_shared<network::TcpServer>(fac, "0.0.0.0", 8080, opt);
    g_server->Start(5);
}

int main()
{
    int n = 0;
    try {
        StartServer();
        while (1) {
            ++n;
            if (n == 30) {
                g_server->StopAccept();
                DPrintf("stop accect");
            }
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            auto s = g_count.ToString();
            DPrintf("gcount: %s", s.c_str());
        }
    } catch (const std::exception& e) {
        WPrintf("exception: %s ", e.what());
    }

    system("pause");
    return 0;
}
