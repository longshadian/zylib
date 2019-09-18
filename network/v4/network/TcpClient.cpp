#include "network/TcpClient.h"

#include <iostream>

TcpClient::TcpClient(std::int64_t index, std::shared_ptr<TcpClientEvent> event, MessageDecoderPtr decoder, const std::string& str_ip, std::uint16_t port)
    : m_event(event)
    , m_decoder(decoder)
    , m_inited()
    , m_io_pool()
    , m_server_addr(boost::asio::ip::address::from_string(str_ip), port)
    , m_is_connected(false)
    , m_index(index)
    , m_channel(std::make_shared<Channel>(event, decoder, m_index, m_index))
{
    m_event->m_client = this;
}

TcpClient::~TcpClient()
{
    m_channel->Shutdown();
    m_io_pool.Stop();
}

bool TcpClient::Start()
{
    if (m_inited) {
        return false;
    }
    if (m_is_connected.exchange(true)) {
        return false;
    }
    if (!m_io_pool.Init(1)) {
        m_is_connected.store(false);
        return false;
    }

    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(m_io_pool.NextIOContext()->m_ioctx);
    socket->async_connect(m_server_addr, [this, socket](const boost::system::error_code& ec) 
        {
            if (ec) {
                m_is_connected.store(false);
                m_event->OnConnect(ec, *m_channel);
                return;
            }
            m_is_connected.store(true);
            m_event->OnConnect(ec, *m_channel);
            m_channel->Init(socket);
        });
    return true;
}

bool TcpClient::IsConnected() const
{
    if (m_inited) {
        return false;
    }
    return m_is_connected;
}

void TcpClient::SendMsg(Message msg)
{
    m_channel->SendMessage(std::move(msg));
}

void TcpClient::Close()
{
    if (!m_is_connected.exchange(false))
        return;
    m_channel->Shutdown();
}

bool TcpClient::Reconnect()
{
    if (m_is_connected.exchange(true)) {
        return false;
    }

    auto ioc = m_io_pool.NextIOContext();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioc->m_ioctx);
    socket->async_connect(m_server_addr, [this, socket](const boost::system::error_code& ec) 
        {
            if (ec) {
                m_is_connected.store(false);
                m_event->OnConnect(ec, *m_channel);
                return;
            }
            m_is_connected.store(true);
            m_event->OnConnect(ec, *m_channel);
            m_channel->Init(socket);
        });
    return true;
}
