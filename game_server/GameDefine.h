#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#include <cstdint>
#include <limits>
#include <string>

namespace gamedef {

//玩家设置相关
enum { USER_INIT_DIAMOND  = 1000 };            //玩家初始钻石

//每个玩家同时存在房间数
enum { MAX_USER_ROOM_COUNT = 10 };

//玩家创建房间未使用后关闭
enum { ROOM_TIMEOUT_SECONDS = 60 * 1 };


extern const std::string DATABASE_ID_GAME;
extern const std::string DATABASE_ID_STATIS;

extern const std::string REDIS_ID_GAME;

extern const std::string GUID_USERID;

extern const std::string GAME_VERSION;

}

#endif