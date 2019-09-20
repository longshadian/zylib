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

bool TcpClient::Start(std::int32_t n)
{
    m_io_pool.Init(n);
    return true;
}

TcpConnectorPtr TcpClient::CreateConnector()
{
    auto ioc = m_io_pool.NextIOContext();
    auto new_socket = std::make_shared<boost::asio::ip::tcp::socket>(ioc->m_ioctx);
    auto channel = std::make_shared<Channel>(m_event_factory, ChannelOption{});

    auto conn = std::make_shared<TcpConnector>(new_socket, channel, m_event_factory->CreateNetworkEvent());
    return conn;
}

void TcpClient::AsyncConnect(TcpConnectorPtr& conn, std::string host, std::uint16_t port)
{
    conn->Connect(std::move(host), port);
}

void TcpClient::SyncConnect(TcpConnectorPtr& conn, std::string host, std::uint16_t port)
{

}

void TcpClient::SyncConnectWaitFor(TcpConnectorPtr& conn, std::string host, std::uint16_t port, std::chrono::seconds sec)
{

}

} // namespace network
