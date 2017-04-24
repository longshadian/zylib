#include "RouterService.h"

#include <chrono>
#include <map>

#include "zylib/Tools.h"

#include "Log.h"
#include "Config.h"


SEndpoint::SEndpoint(std::string ip, int port)
    : m_ip(std::move(ip))
    , m_port(port)
{
}

SEndpoint::~SEndpoint()
{
}

void SEndpoint::setState(STATE s)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_state = s;
}

SEndpoint::STATE SEndpoint::getState() const
{
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_state;
}

bool SEndpoint::connect()
{
    if (getState() != STATE::DISCONNECT)
        return false;
    setState(STATE::CONNECTING);
    m_client = std::make_shared<AsyncClient>(m_io_service, m_resolver.resolve(m_ip, m_port));
    return true;
}

void SEndpoint::close()
{
    m_client->shutdown();
    m_client = nullptr;
}

void SEndpoint::sendMsg(SMessage msg)
{
    auto s = getState();
    if (s == STATE::CONNECTING) {
        return;
    } else if (s == STATE::DISCONNECT) {
        return;
    } else if (s == STATE::CONNECTED) {
        Message network_msg{msg};
        m_client->write(std::move(network_msg));
    }
}

RouterService::RouterService()
{
}

RouterService::~RouterService()
{
}

bool RouterService::init()
{
}

void RouterService::heartbeat(uint32_t diff)
{
}

void RouterService::routerMsg(const std::string& server_name, SMessage msg)
{
    auto endpoint = findEndpoint(server_name); 
    if (!endpoint) {
        endpoint = std::make_shared<SEndpoint>("", 0);
        endpoint->connect();
        m_server_endpoints[server_name] = endpoint;
        return;
    }
    endpoint->sendMsg(std::move(msg));
}

SEndpointPtr RouterService::findEndpoint(const std::string& sname)
{
    auto it = m_server_endpoints.find(sname);
    if (it != m_server_endpoints.end())
        return it->second;
    return nullptr;
}
