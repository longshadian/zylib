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
    TcpConnector(TcpSocketPtr sock, ChannelPtr channel, NetworkEventPtr evt)
        : m_socket(sock)
        , m_channel(channel)
        , m_event(evt)
    {
    }

    ~TcpConnector()
    {
    }

    TcpConnector(const TcpConnector&) = delete;
    TcpConnector& operator=(const TcpConnector&) = delete;
    TcpConnector(TcpConnector&&) = delete;
    TcpConnector& operator=(TcpConnector&&) = delete;

    ChannelPtr& GetChannel()
    {
        return m_channel;
    }

    bool Connected() const
    {
        return m_channel->Opened();
    }

private:
    void AsyncConnect(const std::string& host, std::uint16_t port)
    {
        auto ep = Utilities::CreateEndpoint(host, port);
        m_socket->m_socket.async_connect(ep,
            [this, pthis = shared_from_this()](const boost::system::error_code& ec)
            {
                if (ec) {
                    m_event->OnConnect(ec, *pthis);
                    return;
                }
                m_channel->Init(m_socket);
                m_event->OnConnect(ec, *pthis);
            });
    }

    bool SyncConnect(const std::string& host, std::uint16_t port)
    {
        boost::system::error_code ec{};
        auto ep = Utilities::CreateEndpoint(host, port);
        m_socket->m_socket.connect(ep, ec);
        if (ec) {
            return false;
        } 
        //m_socket->m_socket.non_blocking(true);
        m_channel->Init(m_socket);
        return true;
    }

    bool SyncConnectWaitFor(const std::string& host, std::uint16_t port, std::uint32_t sec)
    {
        if (sec == 0) {
            return SyncConnect(host, port);
        }

        auto pp = std::make_shared<std::promise<std::int32_t>>();
        auto ep = Utilities::CreateEndpoint(host, port);
        m_socket->m_socket.async_connect(ep,
            [this, pthis = shared_from_this(), pp](const boost::system::error_code& ec)
            {
                if (ec) {
                    pp->set_value(ec.value());
                    return;
                }
                pp->set_value(0);
                m_channel->Init(m_socket);
            });

        auto f = pp->get_future();
        std::future_status fs = f.wait_for(std::chrono::seconds{sec});
        if (fs == std::future_status::ready) {
            std::int32_t value = f.get();
            if (value == 0)
                return true;
            return false;
        } else {
            boost::system::error_code ec{};
            m_socket->m_socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
            return false;
        }
    }

private:
    TcpSocketPtr                m_socket;
    ChannelPtr                  m_channel;
    NetworkEventPtr             m_event;
};

} // namespace network
