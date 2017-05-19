#pragma once

#include <vector>
#include <string>

#include "Types.h"

namespace NLNET {

class CUnifiedConnection
{
public:
    struct TEndpoint
    {
        /// 是服务端的链接
        bool m_is_server_conn;
        TcpSocket   m_sock;
    };

public:
    enum class TState 
    { 
        NotUsed, Ready 
    };

    CUnifiedConnection();
    CUnifiedConnection(const std::string& name, TServiceId id);

    void addEndpoint(bool server_conn, TcpSocket sock);
    void connect(const CInetAddress& addr);
public:
    std::string m_ServiceName;
    TState      m_State;
    bool        m_AutoRetry;
    std::vector<TEndpoint> m_Connections;
};


} // NLNET
