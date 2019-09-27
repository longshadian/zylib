#include "network/TcpServer.h"

#include <boost/asio.hpp>

#include "network/Channel.h"
#include "network/Utilities.h"
#include "network/FakeLog.h"

namespace network
{

/**
 * class TcpServer
 ****************************************************************************/
TcpServer::TcpServer(NetworkFactoryPtr fac, std::string host, std::uint16_t port, ServerOption option)
    : m_host(std::move(host))
    , m_port(port)
    , m_option(std::move(option))
    , m_accept_pool()
    , m_io_pool()
    , m_event_factory(fac)
    , m_event(fac->CreateNetworkEvent())
    , m_acceptor()
    , m_listening(false)
{
}

TcpServer::~TcpServer()
{
}

TcpServerPtr TcpServer::Create(NetworkFactoryPtr fac, std::string host, std::uint16_t port, ServerOption opt)
{
    TcpServerPtr p{ new TcpServer(fac, std::move(host), port, std::move(opt)) };
    return p;
}

bool TcpServer::Start(std::int32_t n)
{
    m_accept_pool.Init(1);
    m_io_pool.Init(n);
    if (!InitAcceptor(m_host, m_port))
        return false;
    m_listening.store(true);
    DoAccept();
    return true;
}

void TcpServer::Stop()
{
    StopAccept();
}

void TcpServer::StopAccept()
{
    if (!m_acceptor)
        return;
    if (m_listening.exchange(true))
        return;
    boost::system::error_code ec{};
    m_acceptor->cancel(ec);
    //NETWORK_DPrintf("acceptor cancel: %d %s", ec.value(), ec.message().c_str());
    ec.clear();
    m_acceptor->close(ec);
    //NETWORK_DPrintf("acceptor close: %d %s", ec.value(), ec.message().c_str());
}

void TcpServer::DoAccept()
{
    auto ioc = m_io_pool.NextIOContext();
    auto new_socket = std::make_shared<boost::asio::ip::tcp::socket>(ioc->m_ioctx);

    ChannelOption opt{};
    opt.m_read_timeout_seconds = m_option.m_read_timeout_seconds;
    opt.m_write_timeout_seconds = m_option.m_write_timeout_seconds;
    auto channel = std::make_shared<Channel>(m_event_factory, opt);
    m_acceptor->async_accept(*new_socket,
        [this, new_socket, channel](const boost::system::error_code& ec) 
        { 
            if (ec) {
                m_event->OnAccept(ec, *this, *channel);
            } else {
                channel->Init(new_socket);
                m_event->OnAccept(ec, *this, *channel);
            }
            if (m_listening.load()) {
                DoAccept();
            }
        });
}

const ServerOption& TcpServer::GetOption() const
{
    return m_option;
}

bool TcpServer::InitAcceptor(const std::string& host, std::uint16_t port)
{
    try {
        auto ioc = m_accept_pool.NextIOContext();

        auto ep = Utilities::CreateEndpoint(host, port);
        m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(ioc->m_ioctx);
        m_acceptor->open(ep.protocol());
        m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor->bind(ep);
        m_acceptor->listen();
        return true;
    } catch (const std::exception& e) {
        (void)e;
        return false;
    }
}

} // namespace network
