#pragma once

#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>

#include <boost/asio.hpp>
#include "network/Event.h"
#include "network/IOContextPool.h"
#include "network/Message.h"
#include "network/Channel.h"

class TcpServer;

struct ServerOption
{
    size_t m_max_connection{65535};    //0:unlimited
    size_t m_timeout_seconds{0};       //0:never timeout
};

class TcpServer
{
public:
    TcpServer(std::shared_ptr<NetworkFactory> fac, std::string host, std::uint16_t port, ServerOption option = {});
    ~TcpServer();
    TcpServer(const TcpServer& rhs) = delete;
    TcpServer& operator=(const TcpServer& rhs) = delete;
    TcpServer(TcpServer&& rhs) = delete;
    TcpServer& operator=(TcpServer&& rhs) = delete;

    bool Start();
    void Stop();

    const ServerOption& GetOption() const;
private:
    bool InitAcceptor(const std::string& host, std::uint16_t port);
    void StopAccept();
    void DoAccept();
            
private:
    std::shared_ptr<NetworkFactory>             m_event_factory;
    std::string                                 m_host;
    std::uint16_t                               m_port;
    ServerOption                                m_option;
    std::atomic<bool>                           m_inited;
    IOContextPool                               m_accept_pool;
    IOContextPool                               m_io_pool;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
    std::unordered_set<ChannelPtr>              m_channel_set;
    std::shared_ptr<NetworkEvent>               m_network_event;
};

