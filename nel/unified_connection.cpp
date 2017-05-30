#include "unified_connection.h"

namespace NLNET {


UnifiedConnection::UnifiedConnection()
{

}

void UnifiedConnection::connect(const CInetAddress& addr)
{

}

int32_t UnifiedConnection::findEndpointIndex(AddrID addr_id) const
{
    if (addr_id == AddrID_Default)
        return m_default_endpoint_index;
    if (addr_id >= m_endpoints.size())
        return InvalidEndpointIndex;
    return static_cast<int32_t>(addr_id);
}

void UnifiedConnection::addNewClientScok(TSockPtr client)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_client_sock.push_back(std::move(sock));
}

void UnifiedConnection::update()
{
}

void UnifiedConnection::addReceivedMsg(NetWorkMessagePtr msg)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_new_msg.push_back(std::move(msg));
}

} // NLNET
