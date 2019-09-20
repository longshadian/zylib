#include "network/TcpConnector.h"

#include "network/Utilities.h"

namespace network
{

TcpConnector::TcpConnector(TcpSocketPtr sock, ChannelPtr channel, NetworkEventPtr evt)
    : m_socket(sock)
    , m_channel(channel)
    , m_event(evt)
{
}

TcpConnector::~TcpConnector()
{
}

ChannelPtr& TcpConnector::GetChannel()
{
    return m_channel;
}

void TcpConnector::Connect(std::string host, std::uint16_t port)
{
    auto ep = Utilities::CreateEndpoint(host, port);
    m_socket->async_connect(ep,
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

void TcpConnector::SyncConnect(std::string host, std::uint16_t port, boost::system::error_code* ec)
{
    auto ep = Utilities::CreateEndpoint(host, port);
    m_socket->connect(ep, *ec);
}

void TcpConnector::SyncConnectWaitFor(std::string host, std::uint16_t port, std::uint32_t sec, boost::system::error_code* ec)
{
    if (sec == 0) {
        SyncConnect(std::move(host), port, ec);
        return;
    }

    auto pp = std::make_shared<std::promise<std::int32_t>>();
    auto timer = std::make_shared<boost::asio::steady_timer>(m_socket->get_executor());
    timer->expires_from_now(std::chrono::seconds(sec));
    timer->async_wait([pp](const boost::system::error_code& ec) 
        {
            if (ec) { 
                pp->set_value(ec.value());
            } else {
                pp->set_value(-1);
            }
        });
    auto ep = Utilities::CreateEndpoint(host, port);
    m_socket->async_connect(ep,
        [pp, timer](const boost::system::error_code& ec)
        {
            TimerCancel(timer);
            if (ec) {
                pp->set_value(ec.value());
            } else {
                pp->set_value(0);
            }
        });

    auto f = pp->get_future();
    std::int32_t code = f.get();
    f.wait_for();
}

TimerPtr TcpConnector::CreateConnectTimer(std::uint32_t sec)
{
    auto timer = std::make_shared<boost::asio::steady_timer>(m_socket->get_executor());
    timer->expires_from_now(std::chrono::seconds(sec));
    timer->async_wait([this, pthis = shared_from_this()](const boost::system::error_code& ec) 
        {
            if (!ec) {
                DoClosed(CLOSED_TYPE::TIMEOUT);
            }
        });
    return timer;
}

} // namespace network

