#include "StreamServer.h"
#include <cassert>

#include "RWHandler.h"
#include "FakeLog.h"

StreamServer::StreamServer(boost::asio::io_service& io_service, uint16_t port, const ServerOption& option)
    : m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_socket(io_service)
    , m_handlers()
    , m_option(option)
{
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
        GetCallback().HandlerAcceptOverflow();
        return nullptr;
    }
    HandlerOption opt{};
    opt.m_read_timeout_seconds = m_option.m_timeout_seconds;
    auto handler = std::make_shared<RWHandler>(std::move(m_socket), opt);
    handler->init();
    m_handlers.insert(handler);
    return handler;
}

void StreamServer::acceptCallback(boost::system::error_code ec)
{
    if (!ec) {
        RWHandlerPtr handler = createHandler();
        if (handler) {
            GetCallback().HandlerAccept(handler->getHdl());
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

void StreamServer::stopHandler(const RWHandlerPtr& handler)
{
    m_handlers.erase(handler);
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

void StreamServer::SetCallback(std::unique_ptr<ServerCallback> cb)
{
    m_callback = std::move(cb);
}

ServerCallback& StreamServer::GetCallback()
{
    return *m_callback;
}
