#include "NetClient.h"

namespace NLNET {

NetClient::NetClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_socket(m_io_service)
    , m_is_connected(false)
{
}

NetClient::~NetClient()
{
}

void NetClient::send(CMessage buffer, CUnifiedConnectionPtr /* conn */)
{
    m_io_service.post([this, self = shared_from_this(), buffer = std::move(buffer)]
    {
        bool wait_write = !m_write_msgs.empty();
        m_write_msgs.push_back(std::move(msg));
        if (!wait_write) {
            doWrite();
        }
    });
}

uint64_t NetClient::getReceiveQueueSize()
{
    return 0;
}

uint64_t NetClient::getSendQueueSize()
{
    return 0;
}

void NetClient::displayReceiveQueueStat()
{

}

void NetClient::displaySendQueueStat()
{

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
    m_io_service.post([this, self = shared_from_this()]
    {
        closeSocket();
    });
}

bool NetClient::connect(const CInetAddress& addr)
{
    try {
        boost::asio::ip::tcp::resolver r(m_io_service);
        boost::asio::connect(m_socket, r.resolve({ addr.m_ip, addr.m_port }));
        m_is_connected = true;
        return true;
    } catch (const std::exception& e) {
        LOG_WARNING << "connect to naming service failed:" << e.what();
        return false;
    }
}

void NetClient::doWrite()
{
    boost::asio::async_write(m_socket,
        boost::asio::buffer(m_write_msgs.front().data(), m_write_msgs.front().size()),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (ec) {
                    onSocketError(ec);
                    closeSocket();
                    return;
                }

                m_write_msgs.pop_front();
                if (!m_write_msgs.empty()) {
                    doWrite();
                }
            });
}

void NetClient::onSocketError(const boost::system::error_code& ec)
{
    (void)ec;
}

void NetClient::onConnectionError(const boost::system::error_code& ec)
{
    (void)ec;
}

void NetClient::closeSocket()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
    m_is_connected = false;
}

}
