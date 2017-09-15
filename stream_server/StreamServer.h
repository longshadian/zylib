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
    using CBConnectionOverflow = std::function<void()>;
    using CBConnectionAccept = std::function<void(ConnectionHdl)>;
    using CBConnectionClosed = std::function<void(ConnectionHdl)>;
    using CBConnectionTimeout = std::function<void(ConnectionHdl)>;
    using CBMessageDecode = std::function<void(ConnectionHdl, ByteBuffer& buffer, std::vector<MessagePtr>*)>;
    using CBReceivedMessage = std::function<void(ConnectionHdl, std::vector<MessagePtr>)>;

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

    void setCBConnectOverflow(CBConnectionOverflow cb);
    void setCBConnectAccept(CBConnectionAccept cb);
    void setCBConnectClosed(CBConnectionClosed cb);
    void setCBConnectTimeout(CBConnectionTimeout cb);
    void setCBMessageDecoder(CBMessageDecode cb);
private:
    void stopHandler(const RWHandlerPtr& conn);
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

    void defaultOverflow() const;
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

    CBConnectionOverflow                m_cb_overflow;
    CBConnectionAccept                  m_cb_accept;
    CBConnectionClosed                  m_cb_closed;
    CBConnectionTimeout                 m_cb_timeout;
    CBMessageDecode                     m_cb_msg_decorde;
    CBReceivedMessage                   m_cb_received_msg;
};

}
