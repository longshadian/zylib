#pragma once

#include <memory>
#include <string>
#include <chrono>

#include "WorldDefine.h"
#include "GameType.h"
#include "GameConnection.h"

struct GameMessage
{
    std::chrono::system_clock::time_point m_timestamp{};
    int32_t         m_msg_id;
    std::vector<uint8_t> m_data{};
};

class WorldSession;
using WorldSessionFun = void (WorldSession::*)(GameMessage& msg_req);

struct WorldSessionCB
{
    SESSION_STATE       m_state;
    WorldSessionFun     m_fun;

    operator bool() const
    {
        return m_fun != nullptr;
    }
};

struct GameMessageCB
{
    void visitor(WorldSession* session);

    WorldConnection m_conn{};
    GameMessagePtr  m_msg{nullptr};
    WorldSessionCB  m_session_fun{SESSION_STATE::CLOSED, nullptr};
};

struct FilterGameMessage
{
    bool operator()(const WorldSession& session, const GameMessageCB& cb) const;
};
