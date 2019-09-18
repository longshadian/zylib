#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <boost/asio.hpp>

#include "network/Message.h"
#include "network/Buffer.h"
#include "network/Event.h"

class Channel;
using ChannelHdl = std::weak_ptr<Channel>;
using ChannelPtr = std::shared_ptr<Channel>;

class Channel : public std::enable_shared_from_this<Channel>
{
    using TimerPtr = std::shared_ptr<boost::asio::steady_timer>;

    enum { READ_BUFFER =  1024 * 16 };

public:
    enum class CLOSED_TYPE : int
    {
        NORMAL   = 0,    //正常关闭
        TIMEOUT  = 1,    //超时关闭
        ACTIVITY = 2,    //主动关闭
    };

public:
    Channel(NetworkEventPtr event, MessageDecoderPtr decoder, std::int64_t meta_index, std::int64_t index);
    ~Channel();
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(Channel&&) = delete;

    void                            Init(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void                            SendMessage(Message msg);
    void                            Shutdown();
    ChannelHdl                      Handle();
    std::int64_t                    MetaIndex() const;
    std::int64_t                    Index() const;

private:
    void                            DoClosed(CLOSED_TYPE type = CLOSED_TYPE::NORMAL);
    void                            DoWrite();
    void                            DoRead();
    void                            TimeoutCancel(TimerPtr timer);
    std::size_t                     GetTimeoutTime() const;
    void                            TryDecode();

    TimerPtr                        CreateTimer(std::uint32_t seconds);
private:
    NetworkEventPtr                                 m_event;
    MessageDecoderPtr                               m_decoder;
    std::int64_t                                    m_meta_index;
    std::int64_t                                    m_index;
    std::shared_ptr<NetworkEvent>                   m_callback;
    std::shared_ptr<boost::asio::ip::tcp::socket>   m_socket;
    std::atomic<bool>                               m_is_closed;

    std::list<Message>                              m_write_buffer;
    std::array<std::uint8_t, READ_BUFFER>           m_read_fixed_buffer;
    FlatBuffer                                      m_read_buffer;
};

