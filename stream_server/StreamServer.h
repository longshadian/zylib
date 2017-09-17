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
    size_t m_max_connection{65536};    //0:unlimited
    size_t m_timeout_seconds{0};       //0:never timeout
};

class AsyncServer
{
    friend class RWHandler;
public:

public:
    AsyncServer(boost::asio::io_service& io_service, short port, const ServerOption& optin);
    ~AsyncServer() = default;

    AsyncServer(const AsyncServer& rhs) = delete;
    AsyncServer& operator=(const AsyncServer& rhs) = delete;
    AsyncServer(AsyncServer&& rhs) = delete;
    AsyncServer& operator=(AsyncServer&& rhs) = delete;

    void accept();
    void stop();

    boost::asio::io_service& getIOService();
    const ServerOption& getOption() const;

    void setCBConnectOverflow(CBAcceptOverflow cb);
    void setCBConnectAccept(CBAccept cb);
    void setCBConnectClosed(CBHandlerClosed cb);
    void setCBConnectTimeout(CBHandlerTimeout cb);
    void setCBMessageDecoder(CBMessageDecode cb);
    void setCBReceivedMessage(CBReceivedMessage cb);
private:
    void stopHandler(const ConnectionHdl& hdl);
    void handleAcceptError(const boost::system::error_code& ec);
    void stopAccept();
    RWHandlerPtr createHandler();

    static void refuseAccept(boost::asio::ip::tcp::socket socket);

    void cbAccecpt(ConnectionHdl hdl);
    void cbOverflow();
    void cbTimeout(ConnectionHdl hdl);
    void cbClosed(ConnectionHdl hdl);
    void cbMessageDecode(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    void cbReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> messages);

    void defaultOverflow();
    void defaultAccept(ConnectionHdl hdl);
    void defaultClosed(ConnectionHdl hdl);
    void defaultTimeout(ConnectionHdl hdl);
    void defaultMessageDecode(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    void defaultReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> messages);
private:
    boost::asio::io_service&            m_io_service;
    boost::asio::ip::tcp::acceptor      m_acceptor;
    boost::asio::ip::tcp::socket        m_socket;
    std::unordered_set<RWHandlerPtr>    m_handlers;
    ServerOption                        m_option;

    CBAcceptOverflow                m_cb_overflow;
    CBAccept                  m_cb_accept;
    CBHandlerClosed                  m_cb_closed;
    CBHandlerTimeout                 m_cb_timeout;
    CBMessageDecode                     m_cb_msg_decorde;
    CBReceivedMessage                   m_cb_received_msg;
};

}
