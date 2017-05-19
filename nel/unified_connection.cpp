#include "unified_connection.h"

namespace NLNET {


CUnifiedConnection::CUnifiedConnection()
{

}

CUnifiedConnection::CUnifiedConnection(const std::string& name, TServiceId id)
{

}

void CUnifiedConnection::addEndpoint(bool server_conn, TcpSocket sock)
{
    TEndpoint ep{};
    ep.m_is_server_conn = server_conn;
    ep.m_sock = std::move(sock);
    m_Connections.emplace_back(std::move(ep));
}

void CUnifiedConnection::connect(const CInetAddress& addr)
{

}


} // NLNET
