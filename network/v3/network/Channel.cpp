#include "network/Channel.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "network/Message.h"
#include "network/Event.h"

Channel::Channel(NetworkEventPtr event, MessageDecoderPtr decoder, std::int64_t meta_index, std::int64_t index)
    : m_event(event)
    , m_decoder(decoder)
    , m_meta_index(meta_index)
    , m_index(index)
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

void Channel::Init(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    m_socket = socket;
    DoRead();
}

void Channel::SendMessage(Message msg)
{
    if (!m_socket)
        return;
    m_socket->get_io_context().post([this, pthis = shared_from_this(), msg = std::move(msg)]()
        {
            bool wait_write = !m_write_buffer.empty();
            m_write_buffer.emplace_back(std::move(msg));
            if (!wait_write) {
                DoWrite();
            }
        });
}

void Channel::DoWrite()
{
    boost::asio::async_write(*m_socket
        , boost::asio::buffer(m_write_buffer.front().HeadPtr(), m_write_buffer.front().Length())
        , [this, pthis = shared_from_this()](boost::system::error_code ec, std::size_t length)
        {
            const Message& msg = m_write_buffer.front();
            if (ec) {
                m_event->OnWrite(ec, length, *this, msg);
                DoClosed(CLOSED_TYPE::NORMAL);
                return;
            }
            m_event->OnWrite(ec, length, *this, msg);
            if (!m_write_buffer.empty()) {
                DoWrite();
            }
        });
}

void Channel::DoRead()
{
    std::shared_ptr<boost::asio::steady_timer> timer;
    if (GetTimeoutTime() > 0)
        timer = CreateTimer(GetTimeoutTime());
    m_socket->async_read_some(boost::asio::buffer(m_read_fixed_buffer),
        [this, pthis = shared_from_this(), timer](boost::system::error_code ec, std::size_t read_length)
        {
            timeoutCancel(timer);
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
}

void Channel::Shutdown()
{
    if (!m_socket)
        return;
    m_socket->get_io_context().post([this, pthis = shared_from_this()]() 
        {
            DoClosed(CLOSED_TYPE::ACTIVITY);
        });
}

Channel::TimerPtr Channel::CreateTimer(std::uint32_t seconds)
{
    auto timer = std::make_shared<boost::asio::steady_timer>(m_socket->get_io_context());
    timer->expires_from_now(std::chrono::seconds(seconds));
    timer->async_wait([this, pthis = shared_from_this()](const boost::system::error_code& ec) {
        if (!ec) {
            DoClosed(CLOSED_TYPE::TIMEOUT);
        }
    });
    return timer;
}

void Channel::timeoutCancel(TimerPtr timer)
{
    if (timer) {
        boost::system::error_code ec;
        timer->cancel(ec);
    }
}

/*
boost::asio::io_context& Channel::IOContext()
{
    m_socket->get_executor().post();
    return m_socket->get_executor().
}
*/

ChannelHdl Channel::Handle()
{
    return ChannelHdl(shared_from_this());
}

std::int64_t Channel::MetaIndex() const
{
    return m_meta_index;
}

std::int64_t Channel::Index() const
{
    return m_index;
}

std::size_t Channel::GetTimeoutTime() const
{
    // TODO
    return 0;
    //return m_stream_server.getOption().m_timeout_seconds;
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
