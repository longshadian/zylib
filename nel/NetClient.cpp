#include "NetClient.h"

#include "Log.h"
#include "TSock.h"

namespace NLNET {

NetClient::NetClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_sock(std::make_shared<TSock>(m_io_service, boost::asio::ip::tcp::socket(m_io_service)))
    , m_write_msgs()
    , m_is_connected()
{
}

NetClient::~NetClient()
{
}

void NetClient::send(CMessage msg, TSockPtr /* sock */)
{
    auto self = std::dynamic_pointer_cast<NetClient>(shared_from_this());
    m_io_service.post([this, self, msg = std::move(msg)]
    {
        m_sock->sendMsg(std::move(msg));
    });
}

bool NetClient::flush(CUnifiedConnectionPtr conn)
{
    (void)conn;
    return true;
}

void NetClient::update(DiffTime diff_time)
{
    (void)diff_time;
}

bool NetClient::connected() const
{
    return m_is_connected;
}

void NetClient::disconnect(CUnifiedConnectionPtr /* conn */)
{
    m_sock->shutdown();
}

bool NetClient::connect(const CInetAddress& addr)
{
    return m_sock->connect(addr.m_ip, addr.m_port);
}

}
