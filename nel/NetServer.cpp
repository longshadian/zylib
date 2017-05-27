#include "NetServer.h"

#include "Log.h"
#include "TSock.h"

namespace NLNET {

NetServer::NetServer(boost::asio::io_service& io_service, const std::string& ip, int port, UnifiedConnection& conn)
    : m_io_service(io_service)
    , m_conn(conn)
    , m_acceptor(m_io_service)
    , m_is_connected()
    , m_socket(m_io_service)
    , m_accept_fail_cb()
    , m_accept_success_cb()
{
}

NetServer::~NetServer()
{
}

void NetServer::send(CMessage msg, TSockPtr sock)
{
    m_io_service.post([sock, msg = std::move(msg)]
    {
        sock->sendMsg(std::move(msg));
    });
}

bool NetServer::flush(CUnifiedConnectionPtr conn)
{
    return true;
}

void NetServer::update(DiffTime diff_time)
{

}

bool NetServer::connected() const
{
    return true;
}

void NetServer::disconnect(CUnifiedConnectionPtr conn)
{
}

void NetServer::accept()
{
    auto self = std::dynamic_pointer_cast<NetServer>(shared_from_this());
    m_acceptor.async_accept(m_socket,
        [this, self](const boost::system::error_code& ec)
        {
            if (!ec) {
                auto sock = std::make_shared<TSock>(m_io_service, std::move(m_socket));
                sock->start();
                m_accept_success_cb(sock);
                LOG_DEBUG << "new socket";
            } else {
                LOG_WARNING << "accept error reason:" << ec.value() << " "  << ec.message();
                m_accept_fail_cb(ec);
                return;
            }
            accept();
        });
}

void NetServer::setAcceptFailCB(AcceptFailCB cb)
{
    m_accept_fail_cb = std::move(cb);
}

void NetServer::setAcceptSuccessCB(AcceptSuccessCB cb)
{
    m_accept_success_cb = std::move(cb);
}

}
