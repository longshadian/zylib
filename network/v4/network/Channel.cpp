#include "network/Channel.h"

#include "network/Event.h"
#include "network/Message.h"
#include <chrono>

namespace network {

Channel::Channel(NetworkFactoryPtr fac, ChannelOption opt)
    : m_opt(opt)
    , m_event_factory(fac)
    , m_event(fac->CreateNetworkEvent())
    , m_decoder(fac->CreateMessageDecoder())
    , m_socket()
    , m_opened(false)
    , m_write_buffer()
    , m_read_fixed_buffer()
    , m_read_buffer()
{
}

Channel::~Channel()
{
}

void Channel::Init(TcpSocketPtr socket)
{
    m_opened = true;
    m_socket = socket;
    DoRead();
}

void Channel::SendMsg(Message msg)
{
    if (!m_socket)
        return;
    boost::asio::post(m_socket->m_socket.get_executor(),
        [this, pthis = shared_from_this(), msg = std::move(msg)]() {
            bool wait_write = !m_write_buffer.empty();
            m_write_buffer.emplace_back(std::move(msg));
            if (!wait_write) {
                DoWrite();
            }
        });
}

void Channel::SendMsg(const void* data, std::size_t length)
{
    if (!m_decoder)
        return;
    network::Message msg {};
    m_decoder->Encode(data, length, &msg);
    SendMsg(std::move(msg));
}

void Channel::SendMsg(const std::string& str)
{
    SendMsg(str.data(), str.size());
}

void Channel::DoWrite()
{
    TimerPtr timer = CreateTimer(m_opt.m_write_timeout_seconds, false);
    boost::asio::async_write(m_socket->m_socket,
        boost::asio::buffer(m_write_buffer.front().HeadPtr(), m_write_buffer.front().Length()),
        [this, pthis = shared_from_this(), timer](boost::system::error_code ec, std::size_t length) {
            if (timer) {
                TimerCancel(timer);
            }

            Message msg = std::move(m_write_buffer.front());
            if (ec) {
                m_event->OnWrite(ec, length, *this, msg);
                DoClosed(ECloseType::Write);
                return;
            }
            m_event->OnWrite(ec, length, *this, msg);
            m_write_buffer.pop_front();
            if (!m_write_buffer.empty()) {
                DoWrite();
            }
        });
}

void Channel::DoRead()
{
    TimerPtr timer = CreateTimer(m_opt.m_read_timeout_seconds, true);
    m_socket->m_socket.async_read_some(boost::asio::buffer(m_read_fixed_buffer),
        [this, pthis = shared_from_this(), timer](boost::system::error_code ec, std::size_t read_length) {
            if (timer) {
                TimerCancel(timer);
            }
            if (ec) {
                m_event->OnRead(ec, read_length, *this);
                DoClosed(ECloseType::Read);
                return;
            }
            m_read_buffer.Append(m_read_fixed_buffer.data(), read_length);
            m_event->OnRead(ec, read_length, *this);
            TryDecode();
            DoRead();
        });
}

void Channel::DoClosed(ECloseType type)
{
    if (!m_opened)
        return;
    m_opened = false;
    boost::system::error_code ec;
    m_socket->m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket->m_socket.close(ec);
    m_event->OnClosed(*this, type);
}

void Channel::Shutdown()
{
    if (!m_socket)
        return;
    boost::asio::post(m_socket->m_socket.get_executor(),
        [this, pthis = shared_from_this()]() {
            DoClosed(ECloseType::Active);
        });
}

TimerPtr Channel::CreateTimer(std::uint32_t seconds, bool read)
{
    if (seconds == 0)
        return nullptr;
    auto timer = std::make_shared<boost::asio::steady_timer>(m_socket->m_socket.get_executor());
    timer->expires_from_now(std::chrono::seconds(seconds));
    timer->async_wait([this, pthis = shared_from_this(), read](const boost::system::error_code& ec) {
        if (!ec) {
            if (read) {
                DoClosed(ECloseType::ReadTimeout);
            } else {
                DoClosed(ECloseType::WriteTimeout);
            }
        }
    });
    return timer;
}

ChannelHdl Channel::Handler()
{
    return ChannelHdl(shared_from_this());
}

bool Channel::Opened() const
{
    return m_opened;
}

std::string Channel::IP() const
{
    if (!m_socket)
        return "";
    try {
        auto ep = m_socket->m_socket.local_endpoint();
        return ep.address().to_string();
    } catch (const std::exception&) {
        return "";
    }
}

std::uint16_t Channel::Port() const
{
    if (!m_socket)
        return 0;
    try {
        auto ep = m_socket->m_socket.local_endpoint();
        return ep.port();
    } catch (const std::exception&) {
        return 0;
    }
}

std::string Channel::RemoteIP() const
{
    if (!m_socket)
        return "";
    try {
        auto ep = m_socket->m_socket.remote_endpoint();
        return ep.address().to_string();
    } catch (const std::exception&) {
        return "";
    }
}

std::uint16_t Channel::RemotePort() const
{
    if (!m_socket)
        return 0;
    try {
        auto ep = m_socket->m_socket.remote_endpoint();
        return ep.port();
    } catch (const std::exception&) {
        return 0;
    }
}

void Channel::TryDecode()
{
    std::vector<Message> out;
    m_decoder->Decode(m_read_buffer, &out);
    if (out.empty()) {
        return;
    }
    m_event->OnReceivedMessage(*this, std::move(out));
}

} // namespace network


