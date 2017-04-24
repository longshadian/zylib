#pragma once

#include <map>
#include "WorldMsg.h"
#include "WorldSession.h"

class WorldMsgDispatcher
{
    struct Slot
    {
        int32_t             m_cmd;      
        SESSION_STATE       m_state;
        WorldSessionFun     m_fun;
    };

public:
    WorldMsgDispatcher();
    ~WorldMsgDispatcher() = default;

    bool init();

    WorldSessionCB findRegCallback(int32_t cmd) const;
private:
    std::vector<Slot>           m_slots;
    std::map<int32_t, WorldSessionCB> m_handles;
};
