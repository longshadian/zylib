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

class StreamClient : public std::enable_shared_from_this<StreamClient>
{
public:
    StreamClient(boost::asio::io_service& io_service, ClientOption opt);
    ~StreamClient();

    bool connect(std::string ip, uint16_t port, CBConnect cb);
    void sendMessage(MessagePtr msg);

    void setCBReceiveMsg(CBReceivedMessage cb);
    void setCBTimeout(CBHandlerTimeout cb);
    void setCBClosed(CBHandlerClosed cb);
    void setCBDecode(CBMessageDecode cb);

private:
    bool asyncConnect();

private:
    boost::asio::io_service&     m_io_service;
    boost::asio::ip::tcp::socket m_socket;
    std::string                  m_ip;
    uint16_t                     m_port;
    ClientOption                 m_option;
    CBConnect                    m_cb_connect;
    RWHandlerPtr                 m_handler;

    CBHandlerClosed     m_cb_closed;
    CBHandlerTimeout    m_cb_timeout;
    CBReceivedMessage   m_cb_receive_mgs;
    CBMessageDecode     m_cb_decode;
};

} // network
