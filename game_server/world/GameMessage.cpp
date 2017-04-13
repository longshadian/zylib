#include "GameMessage.h"

#include "WorldSession.h"


void GameMessageCB::visitor(WorldSession* session)
{
    (session->*m_session_fun.m_fun)(*m_msg);
}

bool FilterGameMessage::operator()(const WorldSession& session, const GameMessageCB& game_msg_cb) const
{
    if (game_msg_cb.m_session_fun.m_state == SESSION_STATE::IGNORE)
        return false;
    return session.getSessionState() != game_msg_cb.m_session_fun.m_state;
}
