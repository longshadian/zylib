#ifndef _WORLD_DEFINE_H_
#define _WORLD_DEFINE_H_

#include "network/RWHandler.h"

using WorldConnection = network::ConnectionHdl;
using WorldConnectionLess = std::owner_less<WorldConnection>;

class WorldSession;
using WorldSessionPtr = std::shared_ptr<WorldSession>;

enum class SESSION_STATE 
{
    CLOSED                  = 0,    //�ر�
    UNLOGIN                 = 1,    //δ��¼
    LOGIN                   = 2,    //�ѵ�¼
    IGNORE                  = 3,    //��״̬
};

#endif
