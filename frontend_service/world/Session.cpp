#include "world/Session.h"

ClientSession::ClientSession(Hdl hdl, ConnID conn_id)
    : m_hdl(std::move(hdl))
    , m_conn_id(conn_id)
{

}

ClientSession::~ClientSession()
{

}

uint64_t ClientSession::GetUserID() const
{
    return m_user_id;
}

void ClientSession::SetUserID(uint64_t user_id)
{
    m_user_id = user_id;
}

