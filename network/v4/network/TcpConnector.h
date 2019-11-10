#pragma once

#include <cstdint>

#include "network/Define.h"
#include "network/Channel.h"
#include "network/Event.h"
#include "network/Timer.h"

namespace network
{

class TcpClient;
class TcpConnector;
using TcpConnectorPtr = std::shared_ptr<TcpConnector>;

class TcpConnector : public std::enable_shared_from_this<TcpConnector>
{
    friend class TcpClient;
public:
    TcpConnector(TcpSocketPtr sock, ChannelPtr channel, NetworkEventPtr evt);
    ~TcpConnector();

    TcpConnector(const TcpConnector&) = delete;
    TcpConnector& operator=(const TcpConnector&) = delete;
    TcpConnector(TcpConnector&&) = delete;
    TcpConnector& operator=(TcpConnector&&) = delete;

    ChannelPtr& GetChannel();
    bool Connected() const; 
private:
    void AsyncConnect(const std::string& host, std::uint16_t port);
    bool SyncConnect(const std::string& host, std::uint16_t port);
    bool SyncConnectWaitFor(const std::string& host, std::uint16_t port, std::uint32_t sec);

private:
    TcpSocketPtr                m_socket;
    ChannelPtr                  m_channel;
    NetworkEventPtr             m_event;
};

} // namespace network
