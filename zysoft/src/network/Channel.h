#pragma once

#include <array>
#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <memory>

#include "network/Buffer.h"
#include "network/Define.h"
#include "network/Event.h"
#include "network/Message.h"
#include "network/MessageDecoder.h"
#include "network/TcpSocket.h"
#include "network/Timer.h"

namespace network {

class Channel;
using ChannelHdl = std::weak_ptr<Channel>;
using ChannelPtr = std::shared_ptr<Channel>;

struct ChannelOption {
    std::uint32_t m_read_timeout_seconds {};
    std::uint32_t m_write_timeout_seconds {};
};

class Channel : public std::enable_shared_from_this<Channel> {
    enum { READ_BUFFER = 1024 * 16 };

public:
    Channel(NetworkFactoryPtr fac, ChannelOption opt)
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

    ~Channel()
    {
    }

    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(Channel&&) = delete;

    void Init(TcpSocketPtr socket)
    {
        m_opened = true;
        m_socket = socket;
        DoRead();
    }

    void SendMsg(Message msg)
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

    void SendMsg(const void* data, std::size_t length)
    {
        if (!m_decoder)
            return;
        network::Message msg {};
        m_decoder->Encode(data, length, &msg);
        SendMsg(std::move(msg));
    }

    void SendMsg(const std::string& str)
    {
        SendMsg(str.data(), str.size());
    }

    void Shutdown()
    {
        if (!m_socket)
            return;
        boost::asio::post(m_socket->m_socket.get_executor(),
            [this, pthis = shared_from_this()]() {
                DoClosed(ECloseType::Active);
            });
    }

    ChannelHdl Handler()
    {
        return ChannelHdl(shared_from_this());
    }

    bool Opened() const
    {
        return m_opened;
    }

    std::string IP() const
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

    std::uint16_t Port() const
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

    std::string RemoteIP() const
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

    std::uint16_t RemotePort() const
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


private:
    void DoClosed(ECloseType type)
    {
        if (!m_opened)
            return;
        m_opened = false;
        boost::system::error_code ec;
        m_socket->m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        m_socket->m_socket.close(ec);
        m_event->OnClosed(*this, type);
    }

    void DoWrite()
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

    void DoRead()
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

    void TryDecode()
    {
        std::vector<Message> out;
        m_decoder->Decode(m_read_buffer, &out);
        if (out.empty()) {
            return;
        }
        m_event->OnReceivedMessage(*this, std::move(out));
    }

    TimerPtr CreateTimer(std::uint32_t seconds, bool read)
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


private:
    ChannelOption m_opt;
    NetworkFactoryPtr m_event_factory;
    NetworkEventPtr m_event;
    MessageDecoderPtr m_decoder;
    TcpSocketPtr m_socket;
    bool m_opened;

    std::list<Message> m_write_buffer;
    std::array<std::uint8_t, READ_BUFFER> m_read_fixed_buffer;
    FlatBuffer m_read_buffer;
};

} // namespace network
