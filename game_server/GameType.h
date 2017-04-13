#ifndef _GAME_TYPE_H_
#define _GAME_TYPE_H_

#include <memory>

class GameMessage;
class GameMessageCB;
class GameUser;
class GameConnection;
class GameModule;

class WorldMessage;

class WorldSession;
typedef std::shared_ptr<WorldSession>   WorldSessionPtr;

typedef std::shared_ptr<GameMessage>    GameMessagePtr;
typedef std::shared_ptr<GameMessageCB>  GameMessageCBPtr;
typedef std::shared_ptr<GameConnection> GameConnectionPtr;

typedef std::shared_ptr<GameUser>       GameUserPtr;
typedef std::unique_ptr<GameModule>		GameModulePtr;

using WorldMessagePtr = std::shared_ptr<WorldMessage>;

//Эђ
constexpr
int64_t operator "" _w(unsigned long long v)
{
    return v * 10000;
}

//вк
constexpr
int64_t operator "" _ww(unsigned long long v)
{
    return v * 100000000;
}


#endif
