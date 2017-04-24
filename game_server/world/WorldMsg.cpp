#include "WorldMsg.h"

#include "WorldSession.h"

void WorldMsg::visitor(WorldSession* session)
{
    (session->*m_session_fun.m_fun)(*m_network_msg);
}

bool WorldMsgFilter::operator()(const WorldSession& session, const WorldMsg& world_msg) const
{
    if (world_msg.m_session_fun.m_state == SESSION_STATE::IGNORE)
        return false;
    return session.getSessionState() != world_msg.m_session_fun.m_state;
}
