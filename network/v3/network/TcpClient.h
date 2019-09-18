#pragma once

#include <atomic>
#include <boost/asio.hpp>

#include "network/Message.h"
#include "network/Channel.h"
#include "network/IOContextPool.h"
#include "network/Event.h"

class NetworkEvent;
class MessageDecoder;

class TcpClient
{
public:
    TcpClient(std::int64_t index, std::shared_ptr<TcpClientEvent> event, MessageDecoderPtr decoder, const std::string& str_ip, std::uint16_t port);
    ~TcpClient();
    TcpClient(const TcpClient& rhs) = delete;
    TcpClient& operator=(const TcpClient& rhs) = delete;
    TcpClient(TcpClient&& rhs) = delete;
    TcpClient& operator=(TcpClient&& rhs) = delete;

    bool Start();
    bool IsConnected() const;
    void SendMsg(Message msg);
    void Close();
    bool Reconnect();
private:
    std::shared_ptr<TcpClientEvent> m_event;
    MessageDecoderPtr               m_decoder;
    std::atomic<bool>               m_inited;
    IOContextPool                   m_io_pool;
    boost::asio::ip::tcp::endpoint  m_server_addr;
    std::atomic<bool>               m_is_connected;
    std::int64_t                    m_index;
    ChannelPtr                      m_channel;
};

