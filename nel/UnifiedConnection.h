#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>
#include <unordered_map>

#include "Types.h"
#include "NetClient.h"
#include "NetServer.h"

namespace nlnet {

class UnifiedNetwork;
class TSockContext;

class TEndpoint;
class NetworkMessageContext;

class UnifiedConnection : public std::enable_shared_from_this<UnifiedConnection>
{
public:
    enum class STATE 
    { 
        NotUsed, Ready 
    };

    UnifiedConnection(UnifiedNetwork& network, ServiceID service_id,
        std::string short_name,
        bool auto_retry = false);

    void update(DiffTime diff_time);

    void setServiceName(std::string service_name);
    const std::string& getServiceName() const;

    void setAutoRetry(bool auto_retry); 

    void setServiceID(ServiceID service_id);
    const ServiceID& getServiceID() const;

    void addServerEndpoint(NetServerPtr net_server);
    void addClientEndpoint(NetClientPtr net_client, const CInetAddress& addr);
    bool hasServiceAddr(const CInetAddress& addr) const;

    bool sendMsg(const TSockPtr& sock, CMessage msg);
    bool sendMsg(const CInetAddress& addr, CMessage msg);

    void cbServerAccept(TSockPtr sock);
    void cbClientConnect(boost::system::error_code ec, NetClientPtr net_client);

    NetworkMessageContext createMsgContext(TSockPtr sock) const;
private:
    void cbSockTimeout(TSockPtr sock);
    void cbSockClosed(TSockPtr sock);
    void cbReceivedMsg(NetworkMessagePtr msg);

    void processServerNewSocket();
    void processSockTimeout();
    void processSockClosed();
    void processSockReceivedMsg();
    void processClientAsyncConnectSuccess();
    void processClientAsyncConnectFail();
    void processClientAutoRetry();

    std::shared_ptr<TEndpoint> findEndpoint(const TSockPtr& sock);
    std::shared_ptr<TEndpoint> findEndpoint(const CInetAddress& addr);
    std::shared_ptr<TEndpoint> findRetryEndpoint(const TSockPtr& sock);
    std::shared_ptr<TEndpoint> findRetryEndpoint(const CInetAddress& addr);

    void eraseEndpoint(const TSockPtr& sock);
    void sockBindCallback(TSockPtr sock);
private:
    UnifiedNetwork&         m_network;
    std::string             m_service_name;
    ServiceID               m_service_id;
    STATE                   m_state;
    bool                    m_auto_retry;
    std::unordered_map<TSockPtr, std::shared_ptr<TEndpoint>> m_endpoints;
    std::unordered_map<TSockPtr, std::shared_ptr<TEndpoint>> m_retry_endpoints;
    NetServerPtr            m_server;
    std::vector<CInetAddress> m_client_addrs;

    std::mutex              m_mtx;
    std::list<TSockPtr>     m_closed_sock;
    std::list<TSockPtr>     m_timeout_sock;
    std::list<NetworkMessagePtr> m_new_msg;

    std::list<TSockPtr>     m_server_new_sock;

    std::list<NetClientPtr> m_client_connect_success;
    std::list<NetClientPtr> m_client_connect_fail;
};


} // NLNET
