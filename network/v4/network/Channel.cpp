#include "network/Channel.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "network/Message.h"
#include "network/Event.h"

namespace network
{

Channel::Channel(NetworkFactoryPtr fac, ChannelOption opt)
    : m_opt(opt)
    , m_factory(fac)
    , m_event(fac->CreateNetworkEvent())
    , m_decoder(fac->CreateMessageDecoder())
    , m_socket()
    , m_is_closed(false)
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
    m_socket = socket;
    DoRead();
}

void Channel::SendMsg(Message msg)
{
    if (!m_socket)
        return;
    boost::asio::post(m_socket->get_executor(),
        [this, pthis = shared_from_this(), msg = std::move(msg)]()
        {
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
    network::Message msg{};
    m_decoder->Encode(data, length, &msg);
    SendMsg(msg);
}

void Channel::SendMsg(const std::string& str)
{
    if (!m_decoder)
        return;
    network::Message msg{};
    m_decoder->Encode(str.data(), str.length(), &msg);
    SendMsg(msg);
}

void Channel::DoWrite()
{
    TimerPtr timer = CreateTimer(m_opt.m_write_timeout_seconds);
    boost::asio::async_write(*m_socket
        , boost::asio::buffer(m_write_buffer.front().HeadPtr(), m_write_buffer.front().Length()),
        [this, pthis = shared_from_this(), timer](boost::system::error_code ec, std::size_t length)
        {
            if (timer) {
                TimerCancel(timer);
            }

            Message msg = std::move(m_write_buffer.front());
            if (ec) {
                m_event->OnWrite(ec, length, *this, msg);
                DoClosed(CLOSED_TYPE::NORMAL);
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
    TimerPtr timer = CreateTimer(m_opt.m_read_timeout_seconds);
    m_socket->async_read_some(boost::asio::buffer(m_read_fixed_buffer),
        [this, pthis = shared_from_this(), timer](boost::system::error_code ec, std::size_t read_length)
        {
            if (timer) {
                TimerCancel(timer);
            }
            if (ec) {
                m_event->OnRead(ec, read_length, *this);
                DoClosed();
                return;
            }
            m_read_buffer.Append(m_read_fixed_buffer.data(), read_length);
            m_event->OnRead(ec, read_length, *this);
            TryDecode();
            DoRead();
        });
}

void Channel::DoClosed(CLOSED_TYPE type)
{
    if (m_is_closed.exchange(true))
        return;
    if (type == CLOSED_TYPE::NORMAL) {
    } else if (type == CLOSED_TYPE::TIMEOUT) {
    }
    boost::system::error_code ec;
    m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket->close(ec);
    m_event->OnClosed(*this);
}

void Channel::Shutdown()
{
    if (!m_socket)
        return;
    boost::asio::post(m_socket->get_executor(),
        [this, pthis = shared_from_this()]() 
        {
            DoClosed(CLOSED_TYPE::ACTIVITY);
        });
}

TimerPtr Channel::CreateTimer(std::uint32_t seconds)
{
    if (seconds == 0)
        return nullptr;
    auto timer = std::make_shared<boost::asio::steady_timer>(m_socket->get_executor());
    timer->expires_from_now(std::chrono::seconds(seconds));
    timer->async_wait([this, pthis = shared_from_this()](const boost::system::error_code& ec) 
        {
            if (!ec) {
                DoClosed(CLOSED_TYPE::TIMEOUT);
            }
        });
    return timer;
}

ChannelHdl Channel::Handle()
{
    return ChannelHdl(shared_from_this());
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
