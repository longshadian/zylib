#include "NetServer.h"

#include "Log.h"
#include "TSock.h"
#include "NetClient.h"
#include "UnifiedConnection.h"

namespace NLNET {

NetServer::NetServer(boost::asio::io_service& io_service, const std::string& ip, int port, UnifiedConnection& conn)
    : m_io_service(io_service)
    , m_conn(conn)
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

void NetServer::send(CMessage msg, TSockPtr sock)
{
    m_io_service.post([sock, msgw = std::move(msg)]
    {
        sock->sendMsg(std::move(msgw));
    });
}

bool NetServer::flush(UnifiedConnectionPtr conn)
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

void NetServer::disconnect(UnifiedConnectionPtr conn)
{
}

void NetServer::accept()
{
    auto self = std::dynamic_pointer_cast<NetServer>(shared_from_this());
    m_acceptor.async_accept(m_socket,
        [this, self](const boost::system::error_code& ec)
        {
            // TODO 设置最多能有多少链接
            LOG_DEBUG << "start accept";
            if (!ec) {
                auto sock = std::make_shared<TSock>(std::move(m_socket), m_conn);
                sock->start();
                m_conn.onServerSockAccept(sock);
                //m_accept_success_cb(sock);
                LOG_DEBUG << "new socket";
            } else {
                LOG_WARNING << "accept error. service will stop accept. reason:" 
                    << ec.value() << " "  << ec.message();
                //m_accept_fail_cb(ec);
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
