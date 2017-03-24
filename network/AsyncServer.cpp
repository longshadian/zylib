#include "AsyncServer.h"

#include "RWHandler.h"
#include "FakeLog.h"

AsyncServer::AsyncServer(boost::asio::io_service& io_service, short port, const Option& option)
    : m_io_service(io_service)
    , m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_socket(io_service)
    , m_handlers()
    , m_option(option)
{
}

void AsyncServer::accept()
{
    m_acceptor.async_accept(m_socket, [this](const boost::system::error_code& ec)
        {
            if (!ec) {
                auto handler = createHandler();
                if (handler) {
                    m_handlers.insert(handler);
                    handler->start();
                } else {
                    FAKE_LOG_ERROR() << "too much connection:" << m_handlers.size();
                    refuseAccept(std::move(m_socket));
                }
            } else {
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
    FAKE_LOG_ERROR() << "ERROR error reason:" << ec.value() << " "  << ec.message();
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
    if (m_option.m_max_connection != 0 && (int)m_handlers.size() >= m_option.m_max_connection) {
        return nullptr;
    }

    auto handler = std::make_shared<RWHandler>(m_io_service, std::move(m_socket), m_option.m_timeout_seconds);
    handler->setCallbackOnClosed([this](const RWHandlerPtr& conn) { stopHandler(conn); });
    return handler;
}

void AsyncServer::stopHandler(const RWHandlerPtr& conn)
{
    m_handlers.erase(conn);
    FAKE_LOG_DEBUG() << "current connect count:" << m_handlers.size();
}
