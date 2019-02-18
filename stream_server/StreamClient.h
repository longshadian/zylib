#pragma once

#include <string>
#include <mutex>
#include <chrono>
#include <queue>

#include <boost/asio.hpp>

#include "NetworkType.h"

namespace network {

struct ClientOption
{
    size_t m_read_timeout_seconds;
};

class StreamClient
{
public:
    StreamClient(boost::asio::io_service& io_service, const ClientOption& opt);
    ~StreamClient();
    StreamClient(const StreamClient& rhs) = delete;
    StreamClient& operator=(const StreamClient& rhs) = delete;
    StreamClient(StreamClient&& rhs) = delete;
    StreamClient& operator=(StreamClient&& rhs) = delete;

    bool connect(std::string ip, uint16_t port, CB_Connect cb);
    bool reconnect();
    void sendMessage(MessagePtr msg);
    void shutdown();

    void setCB_ReceivedMessage(CB_ReceivedMessage cb);
    void setCB_MessageDecoder(CB_MessageDecoder cb);
    void setCB_HandlerTimeout(CB_HandlerTimeout cb);
    void setCB_HandlerClosed(CB_HandlerClosed cb);
    Hdl getHdl();

private:
    bool doConnect();
    void doConnectCallback(boost::system::error_code ec);
    void resetConnectSocket();

    static void asyncHandlerClosed_default(Hdl hdl);
    static void asyncHandlerTimeout_Default(Hdl hdl);
    static void asyncMessageDecode_Default(Hdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    static void asyncReceivedMessage_Default(Hdl hdl, std::vector<MessagePtr> messages);

private:
    boost::asio::io_service&     m_io_service;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_connect_socket;
    std::string                  m_ip;
    uint16_t                     m_port;
    ClientOption                 m_option;
    RWHandlerPtr                 m_handler;

    CB_Connect          m_async_connect;
    CB_HandlerClosed    m_async_closed;
    CB_HandlerTimeout   m_async_timeout;
    CB_ReceivedMessage  m_async_receive_mgs;
    CB_MessageDecoder   m_async_msg_decode;
};

} // network
