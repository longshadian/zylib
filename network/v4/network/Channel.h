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
#include "network/MessageDecoder.h"
#include "network/Define.h"
#include "network/Timer.h"

namespace network
{

class Channel;
using ChannelHdl = std::weak_ptr<Channel>;
using ChannelPtr = std::shared_ptr<Channel>;

struct ChannelOption
{
    std::uint32_t m_read_timeout_seconds{};
    std::uint32_t m_write_timeout_seconds{};
};

class Channel : public std::enable_shared_from_this<Channel>
{
    enum { READ_BUFFER =  1024 * 16 };

public:
    Channel(NetworkFactoryPtr fac, ChannelOption opt);
    ~Channel();
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(Channel&&) = delete;

    void                            Init(TcpSocketPtr socket);
    void                            SendMsg(Message msg);
    void                            SendMsg(const void* data, std::size_t length);
    void                            SendMsg(const std::string& str);
    void                            Shutdown();
    ChannelHdl                      Handle();
    bool                            IsOpened() const;

private:
    void                            DoClosed(ECloseType type);
    void                            DoWrite();
    void                            DoRead();
    void                            TryDecode();

    TimerPtr                        CreateTimer(std::uint32_t seconds, bool read);

private:
    ChannelOption                                   m_opt;
    NetworkFactoryPtr                               m_event_factory;
    NetworkEventPtr                                 m_event;
    MessageDecoderPtr                               m_decoder;
    TcpSocketPtr                                    m_socket;
    bool                                            m_is_opened;

    std::list<Message>                              m_write_buffer;
    std::array<std::uint8_t, READ_BUFFER>           m_read_fixed_buffer;
    FlatBuffer                                      m_read_buffer;
};

} // namespace network

