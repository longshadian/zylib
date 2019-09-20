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
    TcpClient(NetworkFactoryPtr fac);
    ~TcpClient();
    TcpClient(const TcpClient& rhs) = delete;
    TcpClient& operator=(const TcpClient& rhs) = delete;
    TcpClient(TcpClient&& rhs) = delete;
    TcpClient& operator=(TcpClient&& rhs) = delete;

    bool Start(std::int32_t n);
    TcpConnectorPtr CreateConnector();

    void AsyncConnect(TcpConnectorPtr& conn, std::string host, std::uint16_t port);
    void SyncConnect(TcpConnectorPtr& conn, std::string host, std::uint16_t port);
    void SyncConnectWaitFor(TcpConnectorPtr& conn, std::string host, std::uint16_t port, std::chrono::seconds sec);

private:
    NetworkFactoryPtr                           m_event_factory;
    NetworkEventPtr                             m_event;
    IOContextPool                               m_io_pool;
};

} // namespace network
