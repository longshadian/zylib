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

class StreamServer
{
public:
    StreamServer(boost::asio::io_service& io_service, short port, const ServerOption& optin);
    ~StreamServer() = default;

    StreamServer(const StreamServer& rhs) = delete;
    StreamServer& operator=(const StreamServer& rhs) = delete;
    StreamServer(StreamServer&& rhs) = delete;
    StreamServer& operator=(StreamServer&& rhs) = delete;

    void accept();
    void stop();

    boost::asio::io_service& getIOService();
    const ServerOption& getOption() const;

    void setCB_ConnectOverflow(CBAcceptOverflow cb);
    void setCB_ConnectAccept(CBAccept cb);
    void setCB_ConnectClosed(CBHandlerClosed cb);
    void setCB_ConnectTimeout(CBHandlerTimeout cb);
    void setCB_MessageDecoder(CBMessageDecode cb);
    void setCB_ReceivedMessage(CBReceivedMessage cb);
private:
    void stopHandler(const ConnectionHdl& hdl);
    void handleAcceptError(const boost::system::error_code& ec);
    void stopAccept();
    RWHandlerPtr createHandler();

    static void refuseAccept(boost::asio::ip::tcp::socket socket);

    void asyncAccecpt(ConnectionHdl hdl);
    void asyncOverflow();
    void asyncTimeout(ConnectionHdl hdl);
    void asyncClosed(ConnectionHdl hdl);
    void asyncMessageDecode(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    void asyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> messages);

    void asyncOverflow_Default();
    void asyncAccept_Default(ConnectionHdl hdl);
    void asyncClosed_default(ConnectionHdl hdl);
    void asyncTimeout_Default(ConnectionHdl hdl);
    void asyncMessageDecode_Default(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    void asyncReceivedMsg_Default(ConnectionHdl hdl, std::vector<MessagePtr> messages);
private:
    boost::asio::io_service&            m_io_service;
    boost::asio::ip::tcp::acceptor      m_acceptor;
    boost::asio::ip::tcp::socket        m_socket;
    std::unordered_set<RWHandlerPtr>    m_handlers;
    ServerOption                        m_option;

    CBAcceptOverflow                    m_cb_overflow;
    CBAccept                            m_cb_accept;
    CBHandlerClosed                     m_cb_closed;
    CBHandlerTimeout                    m_cb_timeout;
    CBMessageDecode                     m_cb_msg_decorde;
    CBReceivedMessage                   m_cb_received_msg;
};

}
