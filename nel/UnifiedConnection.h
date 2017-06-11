#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>
#include <unordered_map>

#include "Types.h"
#include "NetBase.h"
#include "NetClient.h"
#include "NetServer.h"

namespace NLNET {

class UnifiedNetwork;
class TSockContext;

class UnifiedConnection
{
public:
    struct TEndpoint
    {
        TEndpoint() 
            : m_client()
            , m_sock()
            , m_remote_addr()
        {
        }

        /// 已客户端的身份去连接
        TEndpoint(NetClientPtr conn, CInetAddress addr)
            : m_client(conn)
            , m_sock(conn->getSock())
            , m_remote_addr(std::move(addr))
        {
        }

        /// 已服务器的身份,有新的连接到来
        TEndpoint(TSockPtr sock)
            : m_client()
            , m_sock(sock)
            , m_remote_addr()
        {
        }

        bool empty() const
        {
            // TODO
            return m_client == nullptr;
        }

        void shutdown()
        {
            // TODO
        }

        /// 是服务端的链接
        NetClientPtr    m_client;
        TSockPtr        m_sock;

        // 当作客户端去连接服务端
        CInetAddress m_remote_addr;
    };

public:
    enum class STATE 
    { 
        NotUsed, Ready 
    };

    UnifiedConnection(UnifiedNetwork& network, ServiceID service_id,
        std::string short_name,
        bool auto_retry = false);

    void connect(const CInetAddress& addr);

    void update(DiffTime diff_time);

    void setServiceName(std::string service_name);
    const std::string& getServiceName() const;

    void setAutoRetry(bool auto_retry); 

    void setServiceID(ServiceID service_id);
    const ServiceID& getServiceID() const;

    void addAcceptorEndpoint(NetServerPtr net_server);
    void addServiceEndpoint(NetClientPtr net_client, const CInetAddress& addr);
    bool hasServiceAddr(const CInetAddress& addr) const;

    bool sendMsg(const TSockPtr& sock, CMessage msg);
    bool sendMsg(const CInetAddress& addr, CMessage msg);

    void onServerSockAccept(TSockPtr sock);
    void onClientSockConnect(TSockPtr sock);

    TSockContext createSockContext(TSockPtr sock) const;
private:
    void onSockTimeout(TSockPtr sock);
    void onSockClosed(TSockPtr sock);
    void onReceivedMsg(NetWorkMessagePtr msg);

    std::shared_ptr<TEndpoint> findEndpoint(const TSockPtr& sock);
    std::shared_ptr<TEndpoint> findEndpoint(const CInetAddress& addr);
    std::shared_ptr<TEndpoint> findRetryEndpoint(const TSockPtr& sock);
    std::shared_ptr<TEndpoint> findRetryEndpoint(const CInetAddress& addr);

    void eraseEndpoint(const TSockPtr& sock);
private:
    UnifiedNetwork&         m_network;
    std::string             m_service_name;
    ServiceID               m_service_id;
    STATE                   m_state;
    bool                    m_auto_retry;
    std::unordered_map<TSockPtr, std::shared_ptr<TEndpoint>> m_endpoints;
    std::unordered_map<TSockPtr, std::shared_ptr<TEndpoint>> m_retry_endpoints;
    NetServerPtr            m_net_server;
    std::vector<CInetAddress> m_service_addrs;

    std::mutex              m_mtx;
    std::list<TSockPtr>     m_new_client_sock;
    std::list<TSockPtr>     m_closed_sock;
    std::list<TSockPtr>     m_timeout_sock;
    std::list<NetWorkMessagePtr> m_new_msg;
};


} // NLNET
