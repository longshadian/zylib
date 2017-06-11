#include "NetClient.h"

#include <future>

#include "Log.h"
#include "TSock.h"

namespace NLNET {

NetClient::NetClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_sock(std::make_shared<TSock>(boost::asio::ip::tcp::socket(m_io_service)))
    , m_is_connected()
    , m_address() 
    , m_connect_cb()
{
}

NetClient::~NetClient()
{
}

bool NetClient::isConnected() const
{
    return m_is_connected;
}

void NetClient::disconnect()
{
    m_sock->shutdown();
}

bool NetClient::connect(const CInetAddress& addr)
{ 
    m_address = addr;
    m_is_connected = syncConnect(addr.m_ip, addr.m_port);
    if (m_is_connected)
        m_connect_cb(m_sock);
    return m_is_connected;
}

bool NetClient::reconnect()
{
    m_is_connected = syncConnect(m_address.m_ip, m_address.m_port);
    if (m_is_connected)
        m_connect_cb(m_sock);
    return m_is_connected;
}

void NetClient::setConnectCallback(ConnectCallback cb)
{
    m_connect_cb = std::move(cb);
}

TSockPtr& NetClient::getSock()
{
    return m_sock;
}

bool NetClient::syncConnect(const std::string& ip, int32_t port)
{
    std::promise<bool> p{};
    auto f = p.get_future();
    std::thread t([this, &p, &ip, & port] {
        try {
            boost::asio::ip::tcp::resolver r(m_io_service);
            boost::asio::connect(m_sock->getSocket(),
                r.resolve({ip, std::to_string(port)}));
            p.set_value(true);
            m_sock->start();
        } catch (std::exception e) {
            LOG_WARNING << "conn exception " << e.what();
            p.set_value(false);
        }
    });

    t.join();

    try {
        auto ret = f.get();
        return ret;
    } catch (const std::exception& e) {
        return false;
    }
}


}
