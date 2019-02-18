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

    /// �ѿͻ��˵����ȥ����
    TEndpoint(NetClientPtr conn, CInetAddress addr);

    /// �ѷ����������,���µ����ӵ���
    TEndpoint(TSockPtr sock);

    TEndpoint(const TEndpoint& rhs) = delete;
    TEndpoint& operator=(const TEndpoint& rhs) = delete;

    bool isClientSide() const;
    bool isServerSide() const;
    bool empty() const;
    void shutdown();

    /// �����ͻ���ȥ���ӷ����
    CInetAddress    m_remote_addr;
    NetClientPtr    m_client;

    TSockPtr        m_sock;
};

} // nlnet
