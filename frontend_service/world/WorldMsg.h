#pragma once

#include <memory>
#include <string>
#include <chrono>

#include "NetworkMsg.h"
#include "WorldDefine.h"
#include "GameType.h"

class WorldSession;
using WorldSessionFun = void (WorldSession::*)(NetworkMsg& msg_req);

struct WorldSessionCB
{
    SESSION_STATE       m_state;
    WorldSessionFun     m_fun;

    operator bool() const
    {
        return m_fun != nullptr;
    }
};

struct WorldMsg
{
    void visitor(WorldSession* session);

    WorldConnection             m_conn;
    std::shared_ptr<NetworkMsg> m_network_msg;
    WorldSessionCB  m_session_fun{SESSION_STATE::CLOSED, nullptr};
};

struct WorldMsgFilter
{
    bool operator()(const WorldSession& session, const WorldMsg& world_msg) const;
};
