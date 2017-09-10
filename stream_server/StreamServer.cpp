#include "StreamServer.h"

#include "RWHandler.h"
#include "FakeLog.h"

namespace network {

AsyncServer::AsyncServer(boost::asio::io_service& io_service, short port, const ServerOption& option)
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
    m_cb_overflow = std::bind(&AsyncServer::defaultOverflow, this);
    m_cb_accept = std::bind(&AsyncServer::defaultAccept, this, std::placeholders::_1);
    m_cb_closed = std::bind(&AsyncServer::defaultClosed, this, std::placeholders::_1);
    m_cb_timeout = std::bind(&AsyncServer::defaultTimeout, this, std::placeholders::_1);
    m_cb_msg_decorde = std::bind(&AsyncServer::defaultMessageDecode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_cb_received_msg = std::bind(&AsyncServer::defaultReceivedMsg, this, std::placeholders::_1, std::placeholders::_2);
}

void AsyncServer::accept()
{
    m_acceptor.async_accept(m_socket,
        [this](const boost::system::error_code& ec)
        {
            if (!ec) {
                auto handler = createHandler();
                if (handler) {
                    m_handlers.insert(handler);
                    handler->start();
                } else {
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

void AsyncServer::stop()
{
    stopAccept();
    m_io_service.stop();
}

void AsyncServer::handleAcceptError(const boost::system::error_code& ec)
{
    (void)ec;
    stopAccept();
}

void AsyncServer::refuseAccept(boost::asio::ip::tcp::socket socket)
{
    boost::system::error_code ec;
    socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
    socket.close(ec);
}

void AsyncServer::stopAccept()
{
    boost::system::error_code ec;
    m_acceptor.cancel(ec);
    m_acceptor.close(ec);
}

std::shared_ptr<RWHandler> AsyncServer::createHandler()
{
    if (m_option.m_max_connection != 0 && m_handlers.size() >= m_option.m_max_connection) {
        cbOverflow();
        return nullptr;
    }
    return std::make_shared<RWHandler>(*this, std::move(m_socket));
}

void AsyncServer::stopHandler(const RWHandlerPtr& conn)
{
    m_handlers.erase(conn);
    LOG(DEBUG) << "current connect count:" << m_handlers.size();
}

boost::asio::io_service& AsyncServer::getIOService()
{
    return m_io_service;
}

const ServerOption& AsyncServer::getOption() const
{
    return m_option;
}

void AsyncServer::setCBConnectOverflow(CBConnectionOverflow cb)
{
    m_cb_overflow = std::move(cb);
}

void AsyncServer::setCBConnectAccept(CBConnectionAccept cb)
{
    m_cb_accept = std::move(cb);
}

void AsyncServer::setCBConnectClosed(CBConnectionClosed cb)
{
    m_cb_closed = std::move(cb);
}

void AsyncServer::setCBConnectTimeout(CBConnectionTimeout cb)
{
    m_cb_timeout = std::move(cb);
}

void AsyncServer::setCBMessageDecoder(CBMessageDecode cb)
{
    m_cb_msg_decorde = std::move(cb);
}

void AsyncServer::cbAccecpt(ConnectionHdl hdl)
{
    if (m_cb_accept)
        m_cb_accept(std::move(hdl));
}

void AsyncServer::cbOverflow()
{
    if (m_cb_overflow)
        m_cb_overflow();
}

void AsyncServer::cbTimeout(ConnectionHdl hdl)
{
    if (m_cb_timeout)
        m_cb_timeout(std::move(hdl));
}

void AsyncServer::cbClosed(ConnectionHdl hdl)
{
    if (m_cb_timeout)
        m_cb_closed(std::move(hdl));
}

void AsyncServer::cbMessageDecode(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    if (m_cb_msg_decorde)
        m_cb_msg_decorde(std::move(hdl), buffer, out);
}

void AsyncServer::cbReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> messages)
{
    if (m_cb_received_msg)
        m_cb_received_msg(hdl, std::move(messages));
}

void AsyncServer::defaultOverflow() const
{
    LOG(DEBUG) << "refuse accept: too much connection " << m_handlers.size() << "/" << m_option.m_max_connection;
}

void AsyncServer::defaultAccept(ConnectionHdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection accept";
}

void AsyncServer::defaultClosed(ConnectionHdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection closed ";
}

void AsyncServer::defaultTimeout(ConnectionHdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection timeout ";
}

void AsyncServer::defaultMessageDecode(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    (void)hdl;
    (void)buffer;
    (void)out;
    LOG(DEBUG) << "message decoder";
}

void AsyncServer::defaultReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> messages)
{
    (void)hdl;
    (void)messages;
    LOG(DEBUG) << "received message";
}

} // network
