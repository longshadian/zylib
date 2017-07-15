#include "TEndpoint.h"

#include "Log.h"

#include "TSock.h"
#include "NetClient.h"

namespace nlnet {

TEndpoint::TEndpoint()
    : m_remote_addr()
    , m_client()
    , m_sock()
{
}

/// 已客户端的身份去连接
TEndpoint::TEndpoint(NetClientPtr conn, CInetAddress addr)
    : m_remote_addr(std::move(addr))
    , m_client(conn)
    , m_sock(conn->getSock())
{
}

/// 已服务器的身份,有新的连接到来
TEndpoint::TEndpoint(TSockPtr sock)
    : m_remote_addr()
    , m_client()
    , m_sock(sock)
{
}

bool TEndpoint::isClientSide() const
{
    return m_client != nullptr;
}

bool TEndpoint::isServerSide() const
{
    return !isClientSide();
}

bool TEndpoint::empty() const
{
    // TODO
    return m_client == nullptr;
}

void TEndpoint::shutdown()
{
    // TODO
}



} // nlnet
