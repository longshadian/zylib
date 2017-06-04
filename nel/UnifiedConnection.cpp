#include "UnifiedConnection.h"

#include "Log.h"
#include "GAssert.h"

#include "UnifiedNetwork.h"
#include "CallbackManager.h"

namespace NLNET {

UnifiedConnection::UnifiedConnection(UnifiedNetwork& network,
    ServiceID service_id, std::string short_name, bool auto_retry)
    : m_network(network)
    , m_service_name(std::move(short_name))
    , m_service_id(service_id)
    , m_state()
    , m_auto_retry(auto_retry)
    , m_endpoints()
    , m_default_endpoint_index()
    , m_mtx()
    , m_new_client_sock()
    , m_closed_sock()
    , m_timeout_sock()
    , m_new_msg()
{
}

void UnifiedConnection::connect(const CInetAddress& addr)
{

}

int32_t UnifiedConnection::findEndpointIndex(AddrID addr_id) const
{
    if (addr_id == AddrID_Default)
        return m_default_endpoint_index;
    if (addr_id >= m_endpoints.size())
        return InvalidEndpointIndex;
    return static_cast<int32_t>(addr_id);
}

void UnifiedConnection::onServerSockAccept(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_client_sock.push_back(std::move(sock));
}

void UnifiedConnection::onServerSockTimeout(SockID sock_id)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_closed_sock.push_back(sock_id);
}

void UnifiedConnection::onServerSockClosed(SockID sock_id)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timeout_sock.push_back(sock_id);
}

void UnifiedConnection::update(DiffTime diff_time)
{
    // 处理新到来的链接
    std::list<TSockPtr> new_socks{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(new_socks, m_new_client_sock);
    }
    for (auto& it : new_socks) {
        auto* endpoint = findOrCreateEmptyEndpoint();
        endpoint->m_sock = it;
        endpoint->m_is_server_conn = true;
    }
    new_socks.clear();

    // 处理断开的链接
    std::list<SockID> all_closed_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_closed_sock, m_closed_sock);
    }
    for (auto sid : all_closed_sock) {
        auto& endpoint = m_endpoints[sid];
        if (endpoint.empty()) {
            LOG_WARNING << "sid:" << sid << " already closed/timeout";
            continue;
        }
        // TODO 回调
        endpoint.shutdown();
    }
    all_closed_sock.clear();

    // 处理超时的链接
    std::list<SockID> all_timeout_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_timeout_sock, m_timeout_sock);
    }
    for (auto sid : all_timeout_sock) {
        auto& endpoint = m_endpoints[sid];
        if (endpoint.empty()) {
            LOG_WARNING << "sid:" << sid << " already closed/timeout";
            continue;
        }
        // TODO 回调
        endpoint.shutdown();
    }
    all_timeout_sock.clear();

    // TODO 处理收到的消息
    std::list<NetWorkMessagePtr> all_new_msg{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_new_msg, m_new_msg);
    }
    while (!all_new_msg.empty()) {
        auto& msg = all_new_msg.front();
        if (msg->m_sock_id != -1) {
            auto& sock = m_endpoints[msg->m_sock_id].m_sock;
        // 调用回调函数
            auto ret = m_network.getCallbackManager().callbackMsg(sock, msg->m_msg);
            if (!ret) {
                LOG_WARNING << "can't find callback function msg_name:" << msg->m_msg.m_msg_name;
            }
        } else {
            LOG_WARNING << "error sock_id == -1 msg_name:" << msg->m_msg.m_msg_name;
        }
        all_new_msg.pop_front();
    }

    // TODO 处理需要重连的sock
    if (m_auto_retry) {
        for (auto it = m_endpoints.begin(); it != m_endpoints.end(); ++it) {
            auto& endpoint = *it;
            // 当前为空，或者是服务端链接，不需要考虑重连
            if (endpoint.empty() ||
                endpoint.m_is_server_conn ||
                endpoint.m_net_conn->connected())
                continue;
            auto net_client = std::dynamic_pointer_cast<NetClient>(endpoint.m_net_conn);
            net_client->reconnect();
        }
    }
}

void UnifiedConnection::onReceivedMsg(NetWorkMessagePtr msg)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_msg.push_back(std::move(msg));
}

UnifiedConnection::TEndpoint* UnifiedConnection::findEmptyEndpoint()
{
    for (auto& endpoint : m_endpoints) {
        if (endpoint.empty())
            return &endpoint;
    }
    return nullptr;
}

UnifiedConnection::TEndpoint* UnifiedConnection::findOrCreateEmptyEndpoint()
{
    for (auto& endpoint : m_endpoints) {
        if (endpoint.empty())
            return &endpoint;
    }
    auto new_sock_id = static_cast<SockID>(m_endpoints.size());
    m_endpoints.resize(m_endpoints.size() + 1);
    auto* endpoint = &m_endpoints.back();
    endpoint->m_sock_id = new_sock_id;
    return endpoint;
}

void UnifiedConnection::setServiceName(std::string service_name)
{
    m_service_name = std::move(service_name);
}

const std::string& UnifiedConnection::getServiceName() const
{
    return m_service_name;
}

void UnifiedConnection::setAutoRetry(bool auto_retry)
{
    m_auto_retry = auto_retry;
}

void UnifiedConnection::setServiceID(ServiceID service_id)
{
    m_service_id = service_id;
}

ServiceID UnifiedConnection::getServiceID() const
{
    return m_service_id;
}

void UnifiedConnection::addServerAcceptEndpoint(NetServerPtr net_server)
{
    auto* endpoint = findOrCreateEmptyEndpoint();
    GASSERT(endpoint);
    endpoint->m_is_server_conn = true;
    endpoint->m_net_conn = std::move(net_server);
}

void UnifiedConnection::addClientEndpoint(NetClientPtr net_client)
{
    auto* endpoint = findOrCreateEmptyEndpoint();
    GASSERT(endpoint);
    endpoint->m_net_conn = net_client;
}

bool UnifiedConnection::hasEndpoint(const CInetAddress& addr) const
{
    (void)addr;
    return true;
}

UnifiedConnection::TEndpoint* UnifiedConnection::getEndpoint(SockID sock_id)
{
    return &m_endpoints[sock_id];
}

void UnifiedConnection::sendMsg(SockID sock_id, CMessage msg)
{
    auto* endpoint = getEndpoint(sock_id);
    endpoint->m_net_conn->send(msg, endpoint->m_sock);
}

} // NLNET
