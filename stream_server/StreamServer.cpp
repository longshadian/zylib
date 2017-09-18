#include "StreamServer.h"

#include "RWHandler.h"
#include "FakeLog.h"

namespace network {

StreamServer::StreamServer(boost::asio::io_service& io_service, short port, const ServerOption& option)
    : m_io_service(io_service)
    , m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_socket(io_service)
    , m_handlers()
    , m_option(option)
    , m_cb_overflow()
    , m_cb_accept()
    , m_cb_closed()
    , m_cb_timeout()
    , m_cb_msg_decorde()
    , m_cb_received_msg()
{
    // TODO self?? 
    setCB_ConnectOverflow(std::bind(&StreamServer::asyncOverflow_Default, this));
    setCB_ConnectAccept(std::bind(&StreamServer::asyncAccept_Default, this, std::placeholders::_1));
    setCB_ConnectClosed(std::bind(&StreamServer::asyncClosed_default, this, std::placeholders::_1));
    setCB_ConnectTimeout(std::bind(&StreamServer::asyncTimeout_Default, this, std::placeholders::_1));
    setCB_MessageDecoder(std::bind(&StreamServer::asyncMessageDecode_Default, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    setCB_ReceivedMessage(std::bind(&StreamServer::asyncReceivedMsg_Default, this, std::placeholders::_1, std::placeholders::_2));
}

void StreamServer::accept()
{
    m_acceptor.async_accept(m_socket,
        [this](const boost::system::error_code& ec)
        {
            if (!ec) {
                auto handler = createHandler();
                if (!handler) {
                    refuseAccept(std::move(m_socket));
                }
                LOG(DEBUG) << "current handler:" << m_handlers.size();
            } else {
                LOG(DEBUG) << "accept error. server will stop accept. reason:" << ec.value() << " "  << ec.message();
                handleAcceptError(ec);
                return;
            }
            accept();
        });
}

void StreamServer::stop()
{
    stopAccept();
    m_io_service.stop();
}

void StreamServer::handleAcceptError(const boost::system::error_code& ec)
{
    (void)ec;
    stopAccept();
}

void StreamServer::refuseAccept(boost::asio::ip::tcp::socket socket)
{
    boost::system::error_code ec;
    socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
    socket.close(ec);
}

void StreamServer::stopAccept()
{
    boost::system::error_code ec;
    m_acceptor.cancel(ec);
    m_acceptor.close(ec);
}

std::shared_ptr<RWHandler> StreamServer::createHandler()
{
    if (m_option.m_max_connection != 0 && m_handlers.size() >= m_option.m_max_connection) {
        asyncOverflow();
        return nullptr;
    }
    HandlerOption opt{};
    opt.m_read_timeout_seconds = m_option.m_timeout_seconds;
    auto handler = std::make_shared<RWHandler>(std::move(m_socket), opt);

    handler->setCBTimeout(m_cb_timeout);
    handler->setCBClosed(m_cb_closed);
    handler->setCBDecode(m_cb_msg_decorde);
    handler->setCBReceiveMsg(m_cb_received_msg);
    m_handlers.insert(handler);
    return handler;
}

void StreamServer::stopHandler(const ConnectionHdl& hdl)
{
    auto conn = hdl.lock();
    if (conn)
        m_handlers.erase(conn);
    LOG(DEBUG) << "current connect count:" << m_handlers.size();
}

boost::asio::io_service& StreamServer::getIOService()
{
    return m_io_service;
}

const ServerOption& StreamServer::getOption() const
{
    return m_option;
}

void StreamServer::setCB_ConnectOverflow(CBAcceptOverflow cb)
{
    m_cb_overflow = std::move(cb);
}

void StreamServer::setCB_ConnectAccept(CBAccept cb)
{
    m_cb_accept = std::move(cb);
}

void StreamServer::setCB_ConnectClosed(CBHandlerClosed cb)
{
    m_cb_closed = [this, cb_ex = std::move(cb)](ConnectionHdl hdl)
    {
        cb_ex(hdl);
        stopHandler(hdl);
    };
}

void StreamServer::setCB_ConnectTimeout(CBHandlerTimeout cb)
{
    m_cb_timeout = [this, cb_ex = std::move(cb)](ConnectionHdl hdl)
    {
        cb_ex(hdl);
        stopHandler(hdl);
    };
}

void StreamServer::setCB_MessageDecoder(CBMessageDecode cb)
{
    m_cb_msg_decorde = std::move(cb);
}

void StreamServer::setCB_ReceivedMessage(CBReceivedMessage cb)
{
    m_cb_received_msg = std::move(cb);
}

void StreamServer::asyncAccecpt(ConnectionHdl hdl)
{
    if (m_cb_accept)
        m_cb_accept(std::move(hdl));
}

void StreamServer::asyncOverflow()
{
    if (m_cb_overflow)
        m_cb_overflow();
}

void StreamServer::asyncTimeout(ConnectionHdl hdl)
{
    if (m_cb_timeout)
        m_cb_timeout(std::move(hdl));
}

void StreamServer::asyncClosed(ConnectionHdl hdl)
{
    if (m_cb_timeout)
        m_cb_closed(std::move(hdl));
}

void StreamServer::asyncMessageDecode(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    if (m_cb_msg_decorde)
        m_cb_msg_decorde(std::move(hdl), buffer, out);
}

void StreamServer::asyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> messages)
{
    if (m_cb_received_msg)
        m_cb_received_msg(hdl, std::move(messages));
}

void StreamServer::asyncOverflow_Default()
{
    LOG(DEBUG) << "refuse accept: too much connection " << m_handlers.size() << "/" << m_option.m_max_connection;
}

void StreamServer::asyncAccept_Default(ConnectionHdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection accept";
}

void StreamServer::asyncClosed_default(ConnectionHdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection closed ";
}

void StreamServer::asyncTimeout_Default(ConnectionHdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection timeout ";
}

void StreamServer::asyncMessageDecode_Default(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    (void)hdl;
    (void)buffer;
    (void)out;
    LOG(DEBUG) << "message decoder";
}

void StreamServer::asyncReceivedMsg_Default(ConnectionHdl hdl, std::vector<MessagePtr> messages)
{
    (void)hdl;
    (void)messages;
    LOG(DEBUG) << "received message";
}

} // network
