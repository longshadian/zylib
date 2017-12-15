#include "world/Session.h"

uint64_t ClientSession::GetUserID() const
{
    return m_user_id;
}

void ClientSession::SetUserID(uint64_t user_id)
{
    m_user_id = user_id;
}

