#include "network/TcpClient.h"

#include <iostream>

namespace network
{

TcpClient::TcpClient(NetworkFactoryPtr fac)
    : m_event_factory(fac)
    , m_event(fac->CreateNetworkEvent())
    , m_io_pool()
{
}

TcpClient::~TcpClient()
{
}

bool TcpClient::Init(std::int32_t n)
{
    m_io_pool.Init(n);
    return true;
}

TcpConnectorPtr TcpClient::CreateConnector()
{
    auto new_socket = std::make_shared<TcpSocket>(m_io_pool.NextIOContext());
    auto channel = std::make_shared<Channel>(m_event_factory, ChannelOption{});
    auto conn = std::make_shared<TcpConnector>(new_socket, channel, m_event_factory->CreateNetworkEvent());
    return conn;
}

void TcpClient::AsyncConnect(TcpConnectorPtr& conn, const std::string& host, std::uint16_t port)
{
    conn->AsyncConnect(host, port);
}

bool TcpClient::SyncConnect(TcpConnectorPtr& conn, const std::string& host, std::uint16_t port)
{
    return conn->SyncConnect(host, port);
}

bool TcpClient::SyncConnectWaitFor(TcpConnectorPtr& conn, const std::string& host, std::uint16_t port, std::uint32_t sec)
{
    return conn->SyncConnectWaitFor(host, port, sec);
}

} // namespace network
