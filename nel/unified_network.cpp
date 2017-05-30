#include "unified_network.h"

#include "Log.h"

#include <iostream>

#include "unified_connection.h"
#include "NetClient.h"
#include "NetServer.h"
#include "NamingClient.h"
#include "ServiceDefine.h"

namespace NLNET {

UnifiedNetwork::UnifiedNetwork()
    : m_io_service()
    , m_io_work(m_io_service)
    , m_thread()
    , m_connections()
    , m_conn_to_reset()
    , m_used_conn()
    , m_self_service_name()
    , m_self_server_port()
    , m_self_conn()
    , m_initialised()
    , m_naming_client()
{

}

UnifiedNetwork::~UnifiedNetwork()
{
    stop();
    waitThreadExit();
}

void UnifiedNetwork::stop()
{
    m_io_service.stop();
}

void UnifiedNetwork::waitThreadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

bool UnifiedNetwork::init(const std::string& short_name, const CInetAddress& addr)
{
    if (m_initialised)
        return false;

    m_self_server_port = addr.m_port;
    m_self_service_name = short_name;

    // 先启动线程，以下操作都是异步的，需要转换成同步

    // 先监听自己
    auto conn = std::make_shared<UnifiedConnection>();
    std::shared_ptr<NetServer> server{};
    try {
        server = std::make_shared<NetServer>(m_io_service, addr.m_ip, addr.m_port, *conn);
    } catch (const std::exception& e) {
        LOG_WARNING << " netserver listen failed: " << addr.m_ip << ":" << addr.m_port;
        return false;
    }
    server->accept();
    conn->m_service_name = short_name;
    conn->m_auto_retry = true;
    UnifiedConnection::TEndpoint ep{ server };
    conn->m_endpoints.push_back(ep);
    m_self_conn = conn;

    // 连接naming服务器
    if (short_name != NamingServiceName) {
        CInetAddress naming_addr{};
        if (!m_naming_client->connect(naming_addr)) {
            LOG_WARNING << "CUnifiedNetwork init failed: naming service connection failed"; 
            return false;
        }
        // TODO 从NS拉取已经注册的服务器信息
    }

    m_thread = std::move(std::thread([this]
        {
            try {
                m_io_service.run();
            } catch (const std::exception& e) {
                LOG_WARNING << " exception:" << e.what();
            }
        }));

    m_initialised = true;
    return true;
}

void UnifiedNetwork::connect()
{
    // 注册到NS服务器,并且注册到所有已经启动的服务
    auto registered_service = m_naming_client->getRegisterService();
    for (const auto& service : registered_service) {
        // 不要自己连自己
        if (service.m_service_name == m_self_service_name) {
            continue;
        }
        addService(service.m_service_name, service.m_addresses);
    }
}

void UnifiedNetwork::addService(const std::string& name, const std::vector<CInetAddress>& addr, bool auto_retry)
{
    auto conn = findConnection(name);
    if (!conn) {
        conn = std::make_shared<UnifiedConnection>();
        conn->m_service_name = name;
        conn->m_auto_retry = auto_retry;
    }

    for (size_t i = 0; i != addr.size(); ++i) {
        auto client_conn = std::make_shared<NetClient>(m_io_service);
        if (client_conn->connect(addr[i])) {
            UnifiedConnection::TEndpoint ep{client_conn};
            conn->m_endpoints.push_back(ep);
        } else {
            LOG_WARNING << "can't add service because no retry and can't connect";
        } 
    }

    if (addr.size() != conn->m_endpoints.size()) {
        LOG_WARNING << "can't connect to all connections to the service " << addr.size() << "/" << conn->m_endpoints.size();
    }

    LOG_DEBUG << "addService was successful";
}

void UnifiedNetwork::update(DiffTime diff_time)
{
    (void)diff_time;
}

bool UnifiedNetwork::send(SID service_id, CMessage msg, AddrID addr_id)
{
    auto conn = findConnection(service_id);
    if (!conn) {
        LOG_WARNING << "can't send " << msg.getMsgName() << " to the service " << service_id << " because no connection available";
        return false;
    }

    auto idx = conn->findEndpointIndex(addr_id);
    if (idx == InvalidEndpointIndex) {
        LOG_WARNING << "can't send " << msg.getMsgName() << " to the service " << service_id << " because no connection available";
        return false;
    }
    conn->m_endpoints[idx].m_net_conn->send(std::move(msg), conn->m_endpoints[idx].m_sock);
    return true;
}

/*
void UnifiedNetwork::sendAll(const CMessage& msg)
{
}
*/

CUnifiedConnectionPtr UnifiedNetwork::findConnection(const std::string& service_name)
{
    for (auto& it : m_connections) {
        if (it.second->m_service_name == service_name)
            return it.second;
    }
    return nullptr;
}

CUnifiedConnectionPtr UnifiedNetwork::findConnection(const SID& service_id)
{
    auto it = m_connections.find(service_id);
    if (it != m_connections.end()) {
        return it->second;
    }
    return nullptr;
}

int32_t UnifiedNetwork::findEndpointIndex(SID service_id, AddrID add_id) const
{
    auto it = m_connections.find(service_id);
    if (it == m_connections.end())
        return InvalidEndpointIndex;
    const CUnifiedConnectionPtr& conn = it->second;
    if (add_id == AddrID_Default)
        return conn->m_default_endpoint_index;
    if (add_id >= conn->m_endpoints.size())
        return InvalidEndpointIndex;
    return static_cast<int32_t>(add_id);
}

} // NLNET
