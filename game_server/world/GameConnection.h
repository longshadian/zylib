#pragma once

#include <vector>

#include "GameType.h"
#include "eventserver/EventSession.h"

using WorldConnection = EventSessionID;

struct WorldMessage
{
    int32_t              m_msg_id;
    std::vector<uint8_t> m_buffer;
};

struct WorldConnectionInfo
{
    std::string m_ip;
};
