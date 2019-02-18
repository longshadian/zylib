#include "WorldMsgDispatcher.h"

#include "WorldSession.h"

WorldMsgDispatcher::WorldMsgDispatcher()
{
    m_slots = {
        //{int32_t(share::MSG_ID::REQ_REGISTER),  SESSION_STATE::UNLOGIN,     &WorldSession::handleReqRegisert},
        {1,  SESSION_STATE::IGNORE,     &WorldSession::reqPing},
        {2,  SESSION_STATE::IGNORE,     &WorldSession::reqRtt},
        {3,  SESSION_STATE::UNLOGIN,    &WorldSession::reqRegister},
        {4,  SESSION_STATE::UNLOGIN,    &WorldSession::reqLogin},
    };
}

bool WorldMsgDispatcher::init()
{
    for (const auto& it : m_slots) {
        m_handles.insert({it.m_cmd, {it.m_state, it.m_fun}});
    }
    m_slots.clear();
    return true;
}

WorldSessionCB WorldMsgDispatcher::findRegCallback(int32_t cmd) const
{
    auto it = m_handles.find(cmd);
    if (it == m_handles.end()) {
        return {SESSION_STATE::UNLOGIN, nullptr};
    }
    return it->second;
}
