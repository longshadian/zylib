#include "network/TcpServer.h"

#include "GlobalInstance.h"
#include "network/Channel.h"
#include "network/Utilities.h"

/**
 * class TcpServer
 ****************************************************************************/
TcpServer::TcpServer(std::int64_t index, std::shared_ptr<TcpServerEvent> event, MessageDecoderPtr decoder, std::string host, std::uint16_t port, ServerOption option)
    : m_event(event)
    , m_decoder(decoder)
    , m_index(index)
    , m_host(std::move(host))
    , m_port(port)
    , m_option(std::move(option))
    , m_inited()
    , m_accept_pool()
    , m_io_pool()
    , m_acceptor()
    , m_channel_set()
{
    m_event->m_server = this;
}

TcpServer::~TcpServer()
{
    Stop();
}

bool TcpServer::Start()
{
    if (m_inited.exchange(true))
        return false;
    if (!m_accept_pool.Init(1))
        return false;
    if (!m_io_pool.Init(1))
        return false;
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
    boost::system::error_code ec;
    m_acceptor->cancel(ec);
    m_acceptor->close(ec);
    // TODO 是否需要关闭已经创建的连接
}

void TcpServer::DoAccept()
{
    auto ioctx = m_io_pool.NextIOContext();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioctx->m_io_context);
    auto channel = std::make_shared<Channel>(m_event, m_decoder, m_index, GlobalInstance::Get()->NextIndex());
    m_channel_set.emplace(channel);
    m_acceptor->async_accept(*socket,
        [this, socket, channel](const boost::system::error_code& ec) 
        { 
            if (ec) {
                m_event->OnAccept(ec, *this, *channel);
                StopAccept();
                return;
            }
            channel->Init(socket);
            m_event->OnAccept(ec, *this, *channel);
            DoAccept();
        });
}

const ServerOption& TcpServer::getOption() const
{
    return m_option;
}

bool TcpServer::InitAcceptor(const std::string& host, std::uint16_t port)
{
    try {
        auto ioc = m_accept_pool.NextIOContext();
        m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(ioc->m_io_context, Utilities::CreateEndpoint(host, port));
        return true;
    } catch (const std::exception& e) {
        (void)e;
        return false;
    }
}

