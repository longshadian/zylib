#pragma once

#include "Types.h"
#include "Address.h"

namespace nlnet {

class UnifiedNetwork;
class TSockContext;

class TEndpoint
{
public:
    TEndpoint();

    /// 已客户端的身份去连接
    TEndpoint(NetClientPtr conn, CInetAddress addr);

    /// 已服务器的身份,有新的连接到来
    TEndpoint(TSockPtr sock);

    TEndpoint(const TEndpoint& rhs) = delete;
    TEndpoint& operator=(const TEndpoint& rhs) = delete;

    bool isClientSide() const;
    bool isServerSide() const;
    bool empty() const;
    void shutdown();

    /// 当作客户端去连接服务端
    CInetAddress    m_remote_addr;
    NetClientPtr    m_client;

    TSockPtr        m_sock;
};

} // nlnet
