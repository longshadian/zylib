#include "UnifiedConnection.h"

#include "Log.h"
#include "NL_Assert.h"

#include "UnifiedNetwork.h"
#include "CallbackManager.h"
#include "TSock.h"
#include "TEndpoint.h"
#include "CMessage.h"

namespace nlnet {

UnifiedConnection::UnifiedConnection(UnifiedNetwork& network
    , ServiceID service_id, std::string short_name, bool auto_retry)
    : m_network(network)
    , m_service_name(std::move(short_name))
    , m_service_id(service_id)
    , m_state()
    , m_auto_retry(auto_retry)
    , m_endpoints()
    , m_retry_endpoints()
    , m_server()
    , m_client_addrs()
    , m_mtx()
    , m_closed_sock()
    , m_timeout_sock()
    , m_new_msg()
    , m_server_new_sock()
    , m_client_connect_success()
    , m_client_connect_fail()
{
}

void UnifiedConnection::cbServerAccept(TSockPtr sock)
{
    sockBindCallback(sock);
    std::lock_guard<std::mutex> lk{m_mtx};
    m_server_new_sock.push_back(std::move(sock));
}

void UnifiedConnection::cbClientConnect(boost::system::error_code ec, NetClientPtr net_client)
{
    if (ec) {
        std::lock_guard<std::mutex> lk{m_mtx};
        m_client_connect_fail.push_back(net_client);
    } else {
        sockBindCallback(net_client->getSock());
        std::lock_guard<std::mutex> lk{m_mtx};
        m_client_connect_success.push_back(net_client);
    }
}

void UnifiedConnection::cbSockTimeout(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timeout_sock.push_back(sock);
}

void UnifiedConnection::cbSockClosed(TSockPtr sock)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_closed_sock.push_back(sock);
}

void UnifiedConnection::update(DiffTime diff_time)
{
    (void)diff_time;

    processServerNewSocket();

    processSockClosed();
    processSockTimeout();
    processSockReceivedMsg();

    processClientAsyncConnectSuccess();
    processClientAsyncConnectFail();

    if (m_auto_retry) {
        // TODO add timer
        processClientAutoRetry();
    }
}

void UnifiedConnection::cbReceivedMsg(NetworkMessagePtr msg)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_msg.push_back(std::move(msg));
}

void UnifiedConnection::processServerNewSocket()
{
    // 处理新到来的链接
    std::list<TSockPtr> new_socks{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(new_socks, m_server_new_sock);
    }
    for (auto& sock : new_socks) {
        auto endpoint = std::make_shared<TEndpoint>(sock);
        m_endpoints[endpoint->m_sock] = endpoint;

        auto sock_context = createMsgContext(std::move(sock));
        m_network.getCallbackManager().callbackServiceConnect(sock_context);
    }
}

void UnifiedConnection::processSockTimeout()
{
    // 处理超时的链接
    std::list<TSockPtr> all_timeout_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_timeout_sock, m_timeout_sock);
    }
    while (!all_timeout_sock.empty()) {
        auto sock = std::move(all_timeout_sock.front());
        all_timeout_sock.pop_front();
        auto endpoint = findEndpoint(sock);
        NL_ASSERT(endpoint);

        endpoint->shutdown();
        eraseEndpoint(sock);
        if (m_auto_retry) {
            m_retry_endpoints[sock] = endpoint;
        }

        auto sock_context = createMsgContext(std::move(sock));
        m_network.getCallbackManager().callbackServiceDisconnect(sock_context);
    }
}

void UnifiedConnection::processSockClosed()
{
    // 处理断开的链接
    std::list<TSockPtr> all_closed_sock{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_closed_sock, m_closed_sock);
    }
    while (!all_closed_sock.empty()) {
        auto sock = std::move(all_closed_sock.front());
        all_closed_sock.pop_front();
        auto endpoint = findEndpoint(sock);
        NL_ASSERT(endpoint);

        endpoint->shutdown();
        eraseEndpoint(sock);
        if (m_auto_retry) {
            m_retry_endpoints[sock] = endpoint;
        }

        LOG(DEBUG) << "closed sock:" << sock;
        auto sock_context = createMsgContext(std::move(sock));
        m_network.getCallbackManager().callbackServiceDisconnect(sock_context);
    }
}

void UnifiedConnection::processSockReceivedMsg()
{
    // 处理收到的消息
    std::list<NetworkMessagePtr> all_new_msg{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_new_msg, m_new_msg);
    }
    while (!all_new_msg.empty()) {
        auto msg = std::move(all_new_msg.front());
        all_new_msg.pop_front();
        TSockPtr sock = msg->m_sock_hdl.lock();
        if (!sock) {
            LOG(WARNING) << "sock is shutdown. discard msg_name:" << msg->m_msg.m_msg_name;
        } else {
            // 调用回调函数
            auto sock_context = createMsgContext(std::move(sock));
            auto ret = m_network.getCallbackManager().callbackMsg(sock_context, msg->m_msg);
            if (!ret) {
                LOG(WARNING) << "can't find callback function msg_name:" << msg->m_msg.m_msg_name;
            }
        }
    }
}

void UnifiedConnection::processClientAsyncConnectSuccess()
{
    std::list<NetClientPtr> all_clients{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_clients = std::move(m_client_connect_success);
    }
    while (!all_clients.empty()) {
        auto client = std::move(all_clients.front());
        all_clients.pop_front();
        // 连接成功回调
        auto msg_contenxt = createMsgContext(client->getSock());
        m_network.getCallbackManager().callbackServiceConnect(msg_contenxt);
    }
}

void UnifiedConnection::processClientAsyncConnectFail()
{
    std::list<NetClientPtr> all_clients{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_clients = std::move(m_client_connect_fail);
    }
    while (!all_clients.empty()) {
        auto client = std::move(all_clients.front());
        all_clients.pop_front();

        if (m_auto_retry) {
            auto endpoint = findEndpoint(client->getSock());
            NL_ASSERT(endpoint);
            m_retry_endpoints[client->getSock()] = endpoint;
        }
    }
}

void UnifiedConnection::processClientAutoRetry()
{
    // 处理需要重连的sock
    LOG(DEBUG) << "retry true:" << m_retry_endpoints.size();
    for (auto it = m_retry_endpoints.begin(); it != m_retry_endpoints.end();) {
        auto& endpoint = it->second;
        // 客户端连接
        NL_ASSERT(endpoint->isClientSide());
        if (endpoint->m_client->reconnect()) {
            auto sock = endpoint->m_client->getSock();
            m_endpoints[sock] = endpoint;
            it = m_retry_endpoints.erase(it);

            LOG(DEBUG) << "reconn sock:" << sock;

            auto sock_context = createMsgContext(endpoint->m_sock);
            m_network.getCallbackManager().callbackServiceConnect(sock_context);
        } else {
            LOG(WARNING) << "retry faild. sid:" << getServiceID()
                << " service_name:" << getServiceName()
                << " addr: " << endpoint->m_client->getAddress().toString();
            ++it;
        }
    }
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

void UnifiedConnection::addServerEndpoint(NetServerPtr net_server)
{
    m_server = net_server;
}

void UnifiedConnection::addClientEndpoint(NetClientPtr net_client, const CInetAddress& addr)
{
    auto endpoint = std::make_shared<TEndpoint>(net_client, addr);
    m_endpoints[endpoint->m_sock] = endpoint;
    m_client_addrs.push_back(addr);
}

bool UnifiedConnection::hasServiceAddr(const CInetAddress& addr) const
{
    auto it = std::find_if(m_client_addrs.begin(), m_client_addrs.end(), [&addr](const CInetAddress& r)
        {
            return addr.m_ip == r.m_ip && addr.m_port == r.m_port;
        });
    return it != m_client_addrs.end();
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

std::shared_ptr<TEndpoint> UnifiedConnection::findEndpoint(const TSockPtr& sock)
{
    auto it = m_endpoints.find(sock);
    if (it != m_endpoints.end())
        return it->second;
    return nullptr;
}

std::shared_ptr<TEndpoint> UnifiedConnection::findEndpoint(const CInetAddress& addr)
{
    for (auto& ep : m_endpoints) {
        if (ep.second->m_remote_addr == addr)
            return ep.second;
    }
    return nullptr;
}

std::shared_ptr<TEndpoint> UnifiedConnection::findRetryEndpoint(const TSockPtr& sock)
{
    auto it = m_retry_endpoints.find(sock);
    if (it != m_retry_endpoints.end())
        return it->second;
    return nullptr;
}

std::shared_ptr<TEndpoint> UnifiedConnection::findRetryEndpoint(const CInetAddress& addr)
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

NetworkMessageContext UnifiedConnection::createMsgContext(TSockPtr sock) const
{
    // TODO
    (void)sock;
    return NetworkMessageContext{}; //{getServiceName(), getServiceID(), std::move(sock)};
}

void UnifiedConnection::sockBindCallback(TSockPtr sock)
{
    auto self = shared_from_this();
    sock->setReceivedMsgCallback(std::bind(&UnifiedConnection::cbReceivedMsg, self, std::placeholders::_1));
    sock->setClosedCallback(std::bind(&UnifiedConnection::cbSockClosed, self, std::placeholders::_1));
    sock->setTimeoutCallback(std::bind(&UnifiedConnection::cbSockTimeout, self, std::placeholders::_1));
}

} // NLNET
