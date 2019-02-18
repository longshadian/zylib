#ifndef _WORLD_DEFINE_H_
#define _WORLD_DEFINE_H_

#include "network/RWHandler.h"

using WorldConnection = network::ConnectionHdl;
using WorldConnectionLess = std::owner_less<WorldConnection>;

class WorldSession;
using WorldSessionPtr = std::shared_ptr<WorldSession>;

enum class SESSION_STATE 
{
    CLOSED                  = 0,    //¹Ø±Õ
    UNLOGIN                 = 1,    //Î´µÇÂ¼
    LOGIN                   = 2,    //ÒÑµÇÂ¼
    IGNORE                  = 3,    //ÎÞ×´Ì¬
};

#endif
