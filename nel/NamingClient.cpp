#include "NamingClient.h"

#include <future>

#include "Log.h"
#include "TSock.h"
#include "CMessage.h"
#include "Address.h"

namespace nlnet {

NamingClient::NamingClient(boost::asio::io_service& io_service)
    : m_sock(std::make_shared<TSock>(boost::asio::ip::tcp::socket(io_service)))
    , m_is_connected()
{
}

NamingClient::~NamingClient()
{
}

bool NamingClient::isConnected() const
{
    return m_is_connected;
}

bool NamingClient::connect(const CInetAddress& addr)
{
    m_is_connected = syncConnect(addr.m_ip, addr.m_port);
    return m_is_connected;
}

void NamingClient::update()
{

}

std::vector<ServiceAddr> NamingClient::getRegisterService()
{
    // TODO ·µ»ØÔÚNS×¢²áµÄservice
    return {};
}

bool NamingClient::syncConnect(const std::string& ip, int32_t port)
{
    std::promise<bool> p{};
    auto f = p.get_future();
    std::thread t([this, &p, &ip, &port] {
        try {
            boost::asio::ip::tcp::resolver r(m_sock->getIoService());
            boost::asio::connect(m_sock->getSocket(),
                r.resolve({ ip, std::to_string(port) }));
            p.set_value(true);
            m_sock->start();
        } catch (std::exception e) {
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


} // NLNET
