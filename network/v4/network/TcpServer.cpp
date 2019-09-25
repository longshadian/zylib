#include "network/TcpServer.h"

#include "network/Channel.h"
#include "network/Utilities.h"

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
{
}

TcpServer::~TcpServer()
{
    StopAccept();
}

bool TcpServer::Start(std::int32_t n)
{
    m_accept_pool.Init(1);
    m_io_pool.Init(n);
    if (!InitAcceptor(m_host, m_port))
        return false;
    DoAccept();
    return true;
}

void TcpServer::Stop()
{
    StopAccept();
}

void TcpServer::StopAccept()
{
    /*
    // TODO 析构时调用，会奔溃 是否需要关闭已经创建的连接
    if (m_acceptor) {
        boost::system::error_code ec;
        //m_acceptor->cancel(ec);
        m_acceptor->close(ec);
    }
    */
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
                // TODO stop accept??
                m_event->OnAccept(ec, *this, *channel);
                StopAccept();
                return;
            }
            channel->Init(new_socket);
            m_event->OnAccept(ec, *this, *channel);
            DoAccept();
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
        m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(ioc->m_ioctx, Utilities::CreateEndpoint(host, port));
        return true;
    } catch (const std::exception& e) {
        (void)e;
        return false;
    }
}

} // namespace network
