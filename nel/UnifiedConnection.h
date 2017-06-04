#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>

#include "Types.h"
#include "NetBase.h"
#include "NetClient.h"
#include "NetServer.h"

namespace NLNET {

class UnifiedNetwork;

class UnifiedConnection
{
public:
    struct TEndpoint
    {
        TEndpoint()
            : m_is_server_conn()
            , m_net_conn()
            , m_sock_id()
            , m_sock()
        {
        }

        /// 已客户端的身份去连接
        TEndpoint(NetClientPtr conn)
            : m_is_server_conn(false)
            , m_net_conn(conn)
        {
        }

        /// 已服务器的身份等待连接到来
        TEndpoint(NetServerPtr conn)
            : m_is_server_conn(true)
            , m_net_conn(conn)
        {
        }

        /// 已服务器的身份,有新的连接到来
        TEndpoint(TSockPtr sock)
            : m_is_server_conn()
            , m_net_conn()
        {
        }

        bool empty() const
        {
            // TODO
            return m_net_conn == nullptr;
        }

        void shutdown()
        {
            // TODO
        }

        /// 是服务端的链接
        bool        m_is_server_conn;
        NetBasePtr  m_net_conn;
        SockID      m_sock_id;
        TSockPtr    m_sock;
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
    int32_t findEndpointIndex(AddrID addr_id) const;

    void onServerSockAccept(TSockPtr sock);
    void onServerSockTimeout(SockID sock_id);
    void onServerSockClosed(SockID sock_id);
    void onReceivedMsg(NetWorkMessagePtr msg);

    void update(DiffTime diff_time);

    void setServiceName(std::string service_name);
    const std::string& getServiceName() const;

    void setAutoRetry(bool auto_retry); 

    void setServiceID(ServiceID service_id);
    ServiceID getServiceID() const;

    void addServerAcceptEndpoint(NetServerPtr net_server);
    void addClientEndpoint(NetClientPtr net_client);
    bool hasEndpoint(const CInetAddress& addr) const;
    TEndpoint* getEndpoint(SockID sock_id);

    void sendMsg(SockID sock_id, CMessage msg);
private:
    TEndpoint* findEmptyEndpoint();
    TEndpoint* findOrCreateEmptyEndpoint();

private:
    UnifiedNetwork&         m_network;
    std::string             m_service_name;
    ServiceID               m_service_id;
    STATE                   m_state;
    bool                    m_auto_retry;
    std::vector<TEndpoint>  m_endpoints;
    SockID                  m_default_endpoint_index;

    std::mutex              m_mtx;
    std::list<TSockPtr>     m_new_client_sock;
    std::list<SockID>       m_closed_sock;
    std::list<SockID>       m_timeout_sock;
    std::list<NetWorkMessagePtr> m_new_msg;
};


} // NLNET
