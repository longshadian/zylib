#pragma once

#include <cstdint>
#include <atomic>
#include <string>
#include <unordered_set>
#include <boost/asio.hpp>

#include "network/Message.h"
#include "network/Channel.h"
#include "network/IOContextPool.h"
#include "network/Event.h"
#include "network/TcpConnector.h"

namespace network
{

class TcpClient
{
public:
    TcpClient(NetworkFactoryPtr fac)
        : m_event_factory(fac)
        , m_event(fac->CreateNetworkEvent())
        , m_io_pool()
    {
    }

    ~TcpClient()
    {
    }

    TcpClient(const TcpClient& rhs) = delete;
    TcpClient& operator=(const TcpClient& rhs) = delete;
    TcpClient(TcpClient&& rhs) = delete;
    TcpClient& operator=(TcpClient&& rhs) = delete;

    bool Init(std::int32_t n)
    {
        m_io_pool.Init(n);
        return true;
    }

    TcpConnectorPtr CreateConnector()
    {
        auto new_socket = std::make_shared<TcpSocket>(m_io_pool.NextIOContext());
        auto channel = std::make_shared<Channel>(m_event_factory, ChannelOption{});
        auto conn = std::make_shared<TcpConnector>(new_socket, channel, m_event_factory->CreateNetworkEvent());
        return conn;
    }

    void AsyncConnect(TcpConnectorPtr& conn, const std::string& host, std::uint16_t port)
    {
        conn->AsyncConnect(host, port);
    }

    bool SyncConnect(TcpConnectorPtr& conn, const std::string& host, std::uint16_t port)
    {
        return conn->SyncConnect(host, port);
    }

    bool SyncConnectWaitFor(TcpConnectorPtr& conn, const std::string& host, std::uint16_t port, std::uint32_t sec)
    {
        return conn->SyncConnectWaitFor(host, port, sec);
    }

private:
    NetworkFactoryPtr                           m_event_factory;
    NetworkEventPtr                             m_event;
    IOContextPool                               m_io_pool;
};

} // namespace network
