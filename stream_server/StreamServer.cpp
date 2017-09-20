#include "StreamServer.h"
#include <cassert>

#include "RWHandler.h"
#include "FakeLog.h"

namespace network {

StreamServer::StreamServer(boost::asio::io_service& io_service, uint16_t port, const ServerOption& option)
    : m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_socket(io_service)
    , m_handlers()
    , m_option(option)
    , m_async_overflow()
    , m_async_accept()
    , m_async_closed()
    , m_async_timeout()
    , m_async_msg_decorde()
    , m_async_received_msg()
{
    setCB_AcceptOverflow(std::bind(&StreamServer::asyncAcceptOverflow_Default));
    setCB_Accept(std::bind(&StreamServer::asyncAccept_Default, std::placeholders::_1));
    setCB_HandlerClosed(std::bind(&StreamServer::asyncHandlerClosed_default, std::placeholders::_1));
    setCB_HandlerTimeout(std::bind(&StreamServer::asyncHandlerTimeout_Default, std::placeholders::_1));
    setCB_MessageDecoder(std::bind(&StreamServer::asyncMessageDecode_Default, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    setCB_ReceivedMessage(std::bind(&StreamServer::asyncReceivedMessage_Default, std::placeholders::_1, std::placeholders::_2));
}

void StreamServer::accept()
{
    m_acceptor.async_accept(m_socket, std::bind(&StreamServer::acceptCallback, this, std::placeholders::_1));
}

void StreamServer::stop()
{
    stopAccept();
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
        if (m_async_overflow)
            m_async_overflow();
        return nullptr;
    }
    HandlerOption opt{};
    opt.m_read_timeout_seconds = m_option.m_timeout_seconds;
    auto handler = std::make_shared<RWHandler>(std::move(m_socket), opt);
    handler->setCB_AsyncTimeout(m_async_timeout);
    handler->setCB_AsyncClosed(m_async_closed);
    handler->setCB_AsyncDecode(m_async_msg_decorde);
    handler->setCB_AsyncReceiveMsg(m_async_received_msg);
    handler->init();
    m_handlers.insert(handler);
    return handler;
}

void StreamServer::acceptCallback(boost::system::error_code ec)
{
    if (!ec) {
        RWHandlerPtr handler = createHandler();
        if (handler) {
            if (m_async_accept)
                m_async_accept(handler->getHdl());
        } else {
            // refulse accept
            auto this_socket = std::move(m_socket);
            boost::system::error_code ec_2{};
            this_socket.shutdown(boost::asio::socket_base::shutdown_both, ec_2);
            this_socket.close(ec_2);
        }
        LOG(DEBUG) << "current handler:" << m_handlers.size();
    } else {
        LOG(DEBUG) << "accept error. server will stop accept. reason:" << ec.value() << " "  << ec.message();
        stopAccept();
        return;
    }
    accept();
}

void StreamServer::stopHandler(const Hdl& hdl)
{
    auto conn = hdl.lock();
    if (conn)
        m_handlers.erase(conn);
    LOG(DEBUG) << "current connect count:" << m_handlers.size();
}

boost::asio::io_service& StreamServer::getIOService()
{
    return m_acceptor.get_io_service();
}

const ServerOption& StreamServer::getOption() const
{
    return m_option;
}

void StreamServer::setCB_AcceptOverflow(CB_AcceptOverflow cb)
{
    m_async_overflow = std::move(cb);
}

void StreamServer::setCB_Accept(CB_Accept cb)
{
    m_async_accept = std::move(cb);
}

void StreamServer::setCB_HandlerClosed(CB_HandlerClosed cb)
{
    m_async_closed = [this, cb_ex = std::move(cb)](Hdl hdl)
    {
        cb_ex(hdl);
        stopHandler(hdl);
    };
}

void StreamServer::setCB_HandlerTimeout(CB_HandlerTimeout cb)
{
    m_async_timeout = [this, cb_ex = std::move(cb)](Hdl hdl)
    {
        cb_ex(hdl);
        stopHandler(hdl);
    };
}

void StreamServer::setCB_MessageDecoder(CB_MessageDecoder cb)
{
    m_async_msg_decorde = std::move(cb);
}

void StreamServer::setCB_ReceivedMessage(CB_ReceivedMessage cb)
{
    m_async_received_msg = std::move(cb);
}

void StreamServer::asyncAcceptOverflow_Default()
{
    LOG(DEBUG) << "refuse accept: too much connection ";
}

void StreamServer::asyncAccept_Default(Hdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection accept";
}

void StreamServer::asyncHandlerClosed_default(Hdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection closed ";
}

void StreamServer::asyncHandlerTimeout_Default(Hdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection timeout ";
}

void StreamServer::asyncMessageDecode_Default(Hdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    (void)hdl;
    (void)buffer;
    (void)out;
    LOG(DEBUG) << "message decoder";
}

void StreamServer::asyncReceivedMessage_Default(Hdl hdl, std::vector<MessagePtr> messages)
{
    (void)hdl;
    (void)messages;
    LOG(DEBUG) << "received message";
}

} // network
