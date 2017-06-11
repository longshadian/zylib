#include "NetServer.h"

#include "Log.h"
#include "TSock.h"
#include "NetClient.h"

namespace NLNET {

NetServer::NetServer(boost::asio::io_service& io_service, const std::string& ip, int port)
    : m_io_service(io_service)
    , m_acceptor(m_io_service,
        boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), (uint16_t)port})
    , m_is_connected()
    , m_socket(m_io_service)
    , m_accept_fail_cb()
    , m_accept_success_cb()
{
}

NetServer::~NetServer()
{
}

void NetServer::stop()
{
    boost::system::error_code ec{};
    m_acceptor.close(ec);
}

void NetServer::accept()
{
    m_acceptor.async_accept(m_socket,
        [this, self = shared_from_this()](const boost::system::error_code& ec)
        {
            LOG_DEBUG << "start accept";
            if (!ec) {
                auto sock = std::make_shared<TSock>(std::move(m_socket));
                if (m_accept_success_cb)
                    m_accept_success_cb(sock);
                sock->start();
                LOG_DEBUG << "new socket";
            } else {
                LOG_WARNING << "accept error. service will stop accept. reason:" 
                    << ec.value() << " "  << ec.message();
                if (m_accept_fail_cb)
                    m_accept_fail_cb(ec);
                return;
            }
            accept();
        });
}

void NetServer::setAcceptFailCallback(AcceptFailCallback cb)
{
    m_accept_fail_cb = std::move(cb);
}

void NetServer::setAcceptSuccessCallback(AcceptSuccessCallback cb)
{
    m_accept_success_cb = std::move(cb);
}

}
