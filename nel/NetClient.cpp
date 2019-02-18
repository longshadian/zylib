#include "NetClient.h"

#include <future>

#include "Log.h"
#include "TSock.h"

namespace nlnet {

NetClient::NetClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_sock(std::make_shared<TSock>(boost::asio::ip::tcp::socket(m_io_service)))
    , m_inet_addr() 
    , m_connect_cb()
{
}

NetClient::~NetClient()
{
}

int32_t NetClient::getState() const
{
    return m_state;
}

void NetClient::disconnect()
{
    m_sock->shutdown();
}

bool NetClient::connect(const CInetAddress& addr, Connect_Callback cb)
{ 
    if (m_state != DISCONNECT)
        return false;
    m_connect_cb = std::move(cb);
    m_inet_addr = addr;
    return asyncConnect(m_inet_addr);
}

bool NetClient::reconnect()
{
    if (!m_connect_cb)
        return false;
    return asyncConnect(m_inet_addr);
}

const TSockPtr& NetClient::getSock() const
{
    return m_sock;
}

const CInetAddress& NetClient::getAddress() const
{
    return m_inet_addr;
}

bool NetClient::syncConnect(const CInetAddress& addr)
{
    std::promise<bool> p{};
    auto f = p.get_future();
    std::thread t([this, &p, &addr] {
        try {
            boost::asio::ip::tcp::resolver r(m_io_service);
            boost::asio::connect(m_sock->getSocket(),
                r.resolve({ addr.m_ip, std::to_string(addr.m_port) }));
            p.set_value(true);
            m_sock->start();
        }
        catch (std::exception e) {
            NL_LOG(WARNING) << "conn exception " << e.what();
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

bool NetClient::asyncConnect(const CInetAddress& addr)
{
    try {
        boost::asio::ip::address baddr{};
        baddr.from_string(addr.m_ip);
        boost::asio::ip::tcp::endpoint ep_pair{ baddr, addr.m_port };
        m_state = IS_CONNECTING;
        m_sock->getSocket().async_connect(ep_pair,
            [this, self = shared_from_this()](boost::system::error_code ec)
            {
                if (!ec) {
                    NL_LOG(DEBUG) << "connect " << m_inet_addr.toString() << " success";
                    m_sock->start();
                    m_state = CONNECTED;
                } else {
                    NL_LOG(WARNING) << "connect " << m_inet_addr.toString() << " fail " << ec.message();
                    m_state = DISCONNECT;
                }

                if (m_connect_cb)
                    m_connect_cb(ec, self);
            });
            return true;
    } catch (std::exception e) {
        NL_LOG(WARNING) << "conn exception " << e.what();
    }
    return false;
}

}
