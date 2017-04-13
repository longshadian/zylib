#pragma once

#include <map>
#include "GameMessage.h"
#include "WorldSession.h"

class GameMessageDispatcher
{
    struct Slot
    {
        int32_t             m_cmd;      
        SESSION_STATE       m_state;
        WorldSessionFun     m_fun;
    };

    GameMessageDispatcher();
public:
    ~GameMessageDispatcher() = default;
    static GameMessageDispatcher& getInstance();
    bool init();

    WorldSessionCB findRegCallback(int32_t cmd) const;
private:
    std::vector<Slot>           m_slots;
    std::map<int32_t, WorldSessionCB> m_handles;
};
