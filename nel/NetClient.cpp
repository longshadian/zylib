#include "NetClient.h"

#include <future>

#include "Log.h"
#include "TSock.h"

namespace NLNET {

NetClient::NetClient(boost::asio::io_service& io_service,
    UnifiedConnection& conn)
    : m_io_service(io_service)
    , m_sock(std::make_shared<TSock>(boost::asio::ip::tcp::socket(m_io_service), conn))
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

bool NetClient::flush(UnifiedConnectionPtr conn)
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

void NetClient::disconnect(UnifiedConnectionPtr /* conn */)
{
    m_sock->shutdown();
}

bool NetClient::connect(const CInetAddress& addr)
{ 
    m_address = addr;
    return syncConnect(addr.m_ip, addr.m_port);
}

bool NetClient::reconnect()
{
    return syncConnect(m_address.m_ip, m_address.m_port);
}

TSockPtr NetClient::getSock()
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
