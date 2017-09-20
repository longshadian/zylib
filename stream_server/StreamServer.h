#pragma once

#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <memory>
#include <functional>

#include <boost/asio.hpp>

#include "NetworkType.h"

namespace network {

class ByteBuffer;

struct ServerOption
{
    size_t m_max_connection{65535};    //0:unlimited
    size_t m_timeout_seconds{0};       //0:never timeout
};

class StreamServer
{
public:
    StreamServer(boost::asio::io_service& io_service, uint16_t port, const ServerOption& optin);
    ~StreamServer() = default;
    StreamServer(const StreamServer& rhs) = delete;
    StreamServer& operator=(const StreamServer& rhs) = delete;
    StreamServer(StreamServer&& rhs) = delete;
    StreamServer& operator=(StreamServer&& rhs) = delete;

    void accept();
    void stop();

    boost::asio::io_service& getIOService();
    const ServerOption& getOption() const;

    void setCB_AcceptOverflow(CB_AcceptOverflow cb);
    void setCB_Accept(CB_Accept cb);
    void setCB_HandlerClosed(CB_HandlerClosed cb);
    void setCB_HandlerTimeout(CB_HandlerTimeout cb);
    void setCB_MessageDecoder(CB_MessageDecoder cb);
    void setCB_ReceivedMessage(CB_ReceivedMessage cb);
private:
    void acceptCallback(boost::system::error_code ec);
    void stopHandler(const Hdl& hdl);
    void stopAccept();
    RWHandlerPtr createHandler();

    static void asyncAcceptOverflow_Default();
    static void asyncAccept_Default(Hdl hdl);

    static void asyncHandlerClosed_default(Hdl hdl);
    static void asyncHandlerTimeout_Default(Hdl hdl);
    static void asyncMessageDecode_Default(Hdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    static void asyncReceivedMessage_Default(Hdl hdl, std::vector<MessagePtr> messages);
private:
    boost::asio::ip::tcp::acceptor      m_acceptor;
    boost::asio::ip::tcp::socket        m_socket;
    std::unordered_set<RWHandlerPtr>    m_handlers;
    ServerOption                        m_option;

    CB_AcceptOverflow                   m_async_overflow;
    CB_Accept                           m_async_accept;
    CB_HandlerClosed                    m_async_closed;
    CB_HandlerTimeout                   m_async_timeout;
    CB_MessageDecoder                   m_async_msg_decorde;
    CB_ReceivedMessage                  m_async_received_msg;
};

} //network
