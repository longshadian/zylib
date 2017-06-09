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
    , m_mtx()
    , m_new_client_sock()
    , m_closed_sock()
    , m_timeout_sock()
    , m_new_msg()
    , m_net_server()
    , m_addrs()
{
}

void UnifiedConnection::connect(const CInetAddress& addr)
{

}

void UnifiedConnection::onServerSockAccept(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_client_sock.push_back(std::move(sock));
}

void UnifiedConnection::onServerSockTimeout(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_closed_sock.push_back(sock);
}

void UnifiedConnection::onServerSockClosed(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timeout_sock.push_back(sock);
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
        TEndpoint endpoint{};
        endpoint.m_sock = it;
        endpoint.m_is_server_conn = true;
        m_endpoints[endpoint.m_sock] = endpoint;
    }
    new_socks.clear();

    // 处理断开的链接
    std::list<TSockPtr> all_closed_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_closed_sock, m_closed_sock);
    }
    while (!all_closed_sock.empty()) {
        auto sock = all_closed_sock.front();
        auto* endpoint = findEndpoint(sock);
        GASSERT(endpoint);

        // TODO 回调
        endpoint->shutdown();
        eraseEndpoint(sock);
        all_closed_sock.pop_front();
    }

    // 处理超时的链接
    std::list<TSockPtr> all_timeout_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_timeout_sock, m_timeout_sock);
    }

    while (!all_timeout_sock.empty()) {
        auto sock = all_timeout_sock.front();
        auto* endpoint = findEndpoint(sock);
        GASSERT(endpoint);

        // TODO 回调
        endpoint->shutdown();
        eraseEndpoint(sock);
        all_timeout_sock.pop_front();
    }

    // TODO 处理收到的消息
    std::list<NetWorkMessagePtr> all_new_msg{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_new_msg, m_new_msg);
    }
    while (!all_new_msg.empty()) {
        auto& msg = all_new_msg.front();
        TSockPtr sock = msg->m_sock_hdl.lock();
        if (!sock) {
            LOG_WARNING << "sock is shutdown. discard msg_name:" << msg->m_msg.m_msg_name;
        } else {
            // 调用回调函数
            auto ret = m_network.getCallbackManager().callbackMsg(sock, msg->m_msg);
            if (!ret) {
                LOG_WARNING << "can't find callback function msg_name:" << msg->m_msg.m_msg_name;
            }
        }
        all_new_msg.pop_front();
    }

    // TODO 处理需要重连的sock
    if (m_auto_retry) {
        for (auto it = m_endpoints.begin(); it != m_endpoints.end(); ++it) {
            auto& endpoint = it->second;
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
    m_net_server = net_server;
}

void UnifiedConnection::addClientEndpoint(NetClientPtr net_client, const CInetAddress& addr)
{
    auto sock = net_client->getSock();

    TEndpoint endpoint{};
    endpoint.m_is_server_conn = false;
    endpoint.m_net_conn = net_client;
    endpoint.m_sock = sock;
    m_endpoints[sock] = endpoint;

    m_addrs.push_back(addr);
}

bool UnifiedConnection::hasEndpoint(const CInetAddress& addr) const
{
    auto it = std::find_if(m_addrs.begin(), m_addrs.end(), [&addr](const CInetAddress& r)
        {
            return addr.m_ip == r.m_ip && addr.m_port == r.m_port;
        });
    return it != m_addrs.end();
}

void UnifiedConnection::sendMsg(TSockPtr sock, CMessage msg)
{
    auto* endpoint = findEndpoint(sock);
    if (endpoint)
        endpoint->m_net_conn->send(msg, endpoint->m_sock);
}

UnifiedConnection::TEndpoint* UnifiedConnection::findEndpoint(const TSockPtr& sock)
{
    auto it = m_endpoints.find(sock);
    if (it != m_endpoints.end())
        return &it->second;
    return nullptr;
}

void UnifiedConnection::eraseEndpoint(const TSockPtr& sock)
{
    m_endpoints.erase(sock);
}

} // NLNET
