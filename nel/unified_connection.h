#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "Types.h"
#include "NetBase.h"
#include "NetClient.h"
#include "NetServer.h"

namespace NLNET {

class UnifiedConnection
{
public:
    struct TEndpoint
    {
        TEndpoint(std::shared_ptr<NetClient> conn)
            : m_is_server_conn(false)
            , m_net_conn(conn)
        {

        }

        TEndpoint(std::shared_ptr<NetServer> conn)
            : m_is_server_conn(true)
            , m_net_conn(conn)
        {

        }

        /// 是服务端的链接
        bool m_is_server_conn;
        std::shared_ptr<NetBase> m_net_conn;
        TSockPtr m_sock;
    };

public:
    enum class STATE 
    { 
        NotUsed, Ready 
    };

    UnifiedConnection();

    void connect(const CInetAddress& addr);
    int32_t findEndpointIndex(AddrID addr_id) const;

    void addNewClientScok(TSockPtr sock);
    void update();
    void addReceivedMsg(NetWorkMessagePtr msg);
public:
    std::string m_service_name;
    STATE       m_State;
    bool        m_auto_retry;
    std::vector<TEndpoint> m_endpoints;
    int32_t     m_default_endpoint_index;

private:
    std::mutex m_mtx;
    std::list<TSockPtr> m_new_client_sock;
    std::list<NetWorkMessagePtr> m_new_msg;
};


} // NLNET
