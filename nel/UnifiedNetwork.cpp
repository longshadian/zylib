#include "UnifiedNetwork.h"

#include "Log.h"

#include <iostream>

#include "UnifiedConnection.h"
#include "NetClient.h"
#include "NetServer.h"
#include "NamingClient.h"
#include "ServiceDefine.h"
#include "CallbackManager.h"
#include "TSock.h"
#include "CMessage.h"

namespace nlnet {

UnifiedNetwork::UnifiedNetwork()
    : m_io_service()
    , m_io_work(m_io_service)
    , m_thread()
    , m_connections()
    , m_self_service_name()
    , m_self_server_port()
    , m_self_conn()
    , m_initialised()
    , m_naming_client(std::make_unique<NamingClient>(m_io_service))
    , m_cb_manager(std::make_unique<CallbackManager>())
    , m_use_naming_service()
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

bool UnifiedNetwork::init(ServiceID service_id
    , const std::string& short_name
    , const CInetAddress& addr
    , bool use_ns
    )
{
    if (m_initialised)
        return false;

    m_self_server_port = addr.m_port;
    m_self_service_name = short_name;
    m_use_naming_service = use_ns;

    // 先启动线程，以下操作都是异步的，需要转换成同步

    // 先监听自己
    std::shared_ptr<NetServer> server{};
    auto conn = std::make_shared<UnifiedConnection>(*this, service_id,
        std::move(short_name));
    try {
        server = std::make_shared<NetServer>(m_io_service, addr.m_ip, addr.m_port);
    } catch (const std::exception& e) {
        LOG(WARNING) << " netserver listen failed: " << addr.m_ip << ":" << addr.m_port;
        return false;
    }
    server->setAcceptSuccessCallback(
        std::bind(&UnifiedConnection::cbServerAccept, conn, std::placeholders::_1));
    server->accept();
    conn->addServerEndpoint(std::move(server));

    m_connections[conn->getServiceID()] = conn;
    m_self_conn = conn;

    // 连接naming服务器
    if (m_use_naming_service) {
        if (short_name != NamingServiceName) {
            CInetAddress naming_addr{};
            if (!m_naming_client->connect(naming_addr)) {
                LOG(WARNING) << "CUnifiedNetwork init failed: naming service connection failed"; 
                return false;
            }
            // TODO 从NS拉取已经注册的服务器信息
        }
    }

    m_thread = std::move(std::thread([this]
        {
            try {
                m_io_service.run();
            } catch (const std::exception& e) {
                LOG(WARNING) << " exception:" << e.what();
            }
        }));

    m_initialised = true;
    return true;
}

void UnifiedNetwork::connect()
{
    if (!m_use_naming_service)
        return;
    // 注册到NS服务器,并且连接到所有已经启动的服务
    auto registered_service = m_naming_client->getRegisterService();
    for (const auto& service : registered_service) {
        // 不要自己连自己
        if (service.m_service_name == m_self_service_name) {
            continue;
        }
        addService(service.m_service_id, service.m_service_name, service.m_addresses);
    }
}

void UnifiedNetwork::addService(ServiceID service_id
    , const std::string& name
    , const CInetAddress& addr
    , bool auto_retry)
{
    std::vector<CInetAddress> addrs = {addr};
    addService(service_id, name, addrs, auto_retry);
}

void UnifiedNetwork::addService(ServiceID service_id
    , const std::string& name
    , const std::vector<CInetAddress>& addr_list
    , bool auto_retry)
{
    auto conn = findConnection(service_id);
    if (!conn) {
        conn = std::make_shared<UnifiedConnection>(*this, service_id, name, auto_retry);
        m_connections[conn->getServiceID()] = conn;
    }

    for (size_t i = 0; i != addr_list.size(); ++i) {
        // 已经连上service
        if (conn->hasServiceAddr(addr_list[i]))
            continue;
        auto cb = std::bind(&UnifiedConnection::cbClientConnect, conn, std::placeholders::_1, std::placeholders::_2);
        auto net_client = std::make_shared<NetClient>(m_io_service);
        if (net_client->connect(addr_list[i], std::move(cb))) {
            conn->addClientEndpoint(net_client, addr_list[i]);
        } else {
            LOG(WARNING) << "can't add service because no retry and can't connect";
        } 
    }
    LOG(DEBUG) << "addService was successful";
}

void UnifiedNetwork::update(DiffTime diff_time)
{
    for (auto& it : m_connections) {
        auto& conn = it.second;
        conn->update(diff_time);
    }
}

bool UnifiedNetwork::send(ServiceID service_id, CMessage msg, const CInetAddress& addr)
{
    auto conn = findConnection(service_id);
    if (!conn) {
        LOG(WARNING) << "can't send " << msg.getMsgName() << " to the service " << service_id << " because no connection available";
        return false;
    }
    return conn->sendMsg(addr, std::move(msg));
}

/*
void UnifiedNetwork::sendAll(const CMessage& msg)
{
}
*/

UnifiedConnectionPtr UnifiedNetwork::findConnection(const std::string& service_name)
{
    for (auto& it : m_connections) {
        if (it.second->getServiceName() == service_name)
            return it.second;
    }
    return nullptr;
}

UnifiedConnectionPtr UnifiedNetwork::findConnection(const ServiceID& service_id)
{
    auto it = m_connections.find(service_id);
    if (it != m_connections.end()) {
        return it->second;
    }
    return nullptr;
}

CallbackManager& UnifiedNetwork::getCallbackManager()
{
    return *m_cb_manager;
}

void UnifiedNetwork::addConnection(UnifiedConnectionPtr conn)
{
    (void)conn;
}

} // NLNET
