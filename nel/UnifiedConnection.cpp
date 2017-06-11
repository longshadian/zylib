#include "UnifiedConnection.h"

#include "Log.h"
#include "GAssert.h"

#include "UnifiedNetwork.h"
#include "CallbackManager.h"
#include "TSock.h"

namespace NLNET {

UnifiedConnection::UnifiedConnection(UnifiedNetwork& network,
    ServiceID service_id, std::string short_name, bool auto_retry)
    : m_network(network)
    , m_service_name(std::move(short_name))
    , m_service_id(service_id)
    , m_state()
    , m_auto_retry(auto_retry)
    , m_endpoints()
    , m_retry_endpoints()
    , m_net_server()
    , m_service_addrs()
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

void UnifiedConnection::onServerSockAccept(TSockPtr sock)
{
    sock->setReceivedMsgCallback(
        std::bind(&UnifiedConnection::onReceivedMsg,
            this, std::placeholders::_1));
    sock->setClosedCallback(
        std::bind(&UnifiedConnection::onSockClosed,
            this, std::placeholders::_1));
    sock->setTimeoutCallback(
        std::bind(&UnifiedConnection::onSockTimeout,
            this, std::placeholders::_1));
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_client_sock.push_back(std::move(sock));
}

void UnifiedConnection::onClientSockConnect(TSockPtr sock)
{
    sock->setReceivedMsgCallback(
        std::bind(&UnifiedConnection::onReceivedMsg,
            this, std::placeholders::_1));
    sock->setClosedCallback(
        std::bind(&UnifiedConnection::onSockClosed,
            this, std::placeholders::_1));
    sock->setTimeoutCallback(
        std::bind(&UnifiedConnection::onSockTimeout,
            this, std::placeholders::_1));
}

void UnifiedConnection::onSockTimeout(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timeout_sock.push_back(sock);
}

void UnifiedConnection::onSockClosed(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_closed_sock.push_back(sock);
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
        auto endpoint = std::make_shared<TEndpoint>(it);
        m_endpoints[endpoint->m_sock] = endpoint;
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
        auto endpoint = findEndpoint(sock);
        GASSERT(endpoint);

        endpoint->shutdown();
        eraseEndpoint(sock);
        if (m_auto_retry) {
            m_retry_endpoints[sock] = endpoint;
        }

        LOG_DEBUG << "closed sock:" << sock;
        all_closed_sock.pop_front();
        auto sock_context = createSockContext(std::move(sock));
        m_network.getCallbackManager().callbackServiceDown(
            sock_context);
    }

    // 处理超时的链接
    std::list<TSockPtr> all_timeout_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_timeout_sock, m_timeout_sock);
    }
    while (!all_timeout_sock.empty()) {
        auto sock = all_timeout_sock.front();
        auto endpoint = findEndpoint(sock);
        GASSERT(endpoint);

        endpoint->shutdown();
        eraseEndpoint(sock);
        if (m_auto_retry) {
            m_retry_endpoints[sock] = endpoint;
        }
        all_timeout_sock.pop_front();

        auto sock_context = createSockContext(std::move(sock));
        m_network.getCallbackManager().callbackServiceDown(
            sock_context);
    }

    // 处理收到的消息
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
            auto sock_context = createSockContext(std::move(sock));
            auto ret = m_network.getCallbackManager().callbackMsg(
                sock_context, msg->m_msg);
            if (!ret) {
                LOG_WARNING << "can't find callback function msg_name:" << msg->m_msg.m_msg_name;
            }
        }
        all_new_msg.pop_front();
    }

    // 处理需要重连的sock
    if (m_auto_retry) {
        LOG_DEBUG << "retry true:" << m_retry_endpoints.size();
        for (auto it = m_retry_endpoints.begin(); it != m_retry_endpoints.end();) {
            auto& endpoint = it->second;
            if (endpoint->m_client->reconnect()) {
                auto sock = endpoint->m_client->getSock();
                m_endpoints[sock] = endpoint;
                it = m_retry_endpoints.erase(it);

                LOG_DEBUG << "reconn sock:" << sock;

                auto sock_context = createSockContext(endpoint->m_sock);
                m_network.getCallbackManager().callbackServiceUp(
                    sock_context);
            } else {
                LOG_DEBUG << "need retry faild";
                ++it;
            }
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

const ServiceID& UnifiedConnection::getServiceID() const
{
    return m_service_id;
}

void UnifiedConnection::addAcceptorEndpoint(NetServerPtr net_server)
{
    m_net_server = net_server;
}

void UnifiedConnection::addServiceEndpoint(NetClientPtr net_client
    , const CInetAddress& addr)
{
    auto endpoint = std::make_shared<TEndpoint>(net_client, addr);
    m_endpoints[endpoint->m_sock] = endpoint;
    m_service_addrs.push_back(addr);
}

bool UnifiedConnection::hasServiceAddr(const CInetAddress& addr) const
{
    auto it = std::find_if(m_service_addrs.begin(), m_service_addrs.end(), [&addr](const CInetAddress& r)
        {
            return addr.m_ip == r.m_ip && addr.m_port == r.m_port;
        });
    return it != m_service_addrs.end();
}

bool UnifiedConnection::sendMsg(const TSockPtr& sock, CMessage msg)
{
    auto endpoint = findEndpoint(sock);
    if (endpoint)
        return endpoint->m_sock->sendMsg(std::move(msg));
    return false;
}

bool UnifiedConnection::sendMsg(const CInetAddress& addr, CMessage msg)
{
    auto endpoint = findEndpoint(addr);
    if (endpoint) {
        return endpoint->m_sock->sendMsg(std::move(msg));
    }
    return false;
}

std::shared_ptr<UnifiedConnection::TEndpoint>
    UnifiedConnection::findEndpoint(const TSockPtr& sock)
{
    auto it = m_endpoints.find(sock);
    if (it != m_endpoints.end())
        return it->second;
    return nullptr;
}

std::shared_ptr<UnifiedConnection::TEndpoint>
    UnifiedConnection::findEndpoint(const CInetAddress& addr)
{
    for (auto& ep : m_endpoints) {
        if (ep.second->m_remote_addr == addr)
            return ep.second;
    }
    return nullptr;
}

std::shared_ptr<UnifiedConnection::TEndpoint>
    UnifiedConnection::findRetryEndpoint(const TSockPtr& sock)
{
    auto it = m_retry_endpoints.find(sock);
    if (it != m_retry_endpoints.end())
        return it->second;
    return nullptr;
}

std::shared_ptr<UnifiedConnection::TEndpoint>
    UnifiedConnection::findRetryEndpoint(const CInetAddress& addr)
{
    for (auto& ep : m_retry_endpoints) {
        if (ep.second->m_remote_addr == addr)
            return ep.second;
    }
    return nullptr;
}

void UnifiedConnection::eraseEndpoint(const TSockPtr& sock)
{
    m_endpoints.erase(sock);
}

TSockContext UnifiedConnection::createSockContext(TSockPtr sock) const
{
    return TSockContext{getServiceName(), getServiceID(), std::move(sock)};
}

} // NLNET
