#include "NamingClient.h"

#include "Log.h"

namespace NLNET {

NamingClient::NamingClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_sock(io_service)
    , m_is_connected(true)
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
    try {
        boost::asio::ip::tcp::resolver r(m_io_service);
        boost::asio::connect(m_sock, r.resolve({addr.m_ip, std::to_string(addr.m_port)}));
        return true;
    } catch (const std::exception& e) {
        LOG_WARNING << "connect to naming service failed:" << e.what();
        return false;
    }
    m_is_connected = true;
}

void NamingClient::update()
{

}

std::vector<ServiceAddr> NamingClient::getRegisterService()
{
    // TODO ·µ»ØÔÚNS×¢²áµÄservice
    return {};
}


} // NLNET
