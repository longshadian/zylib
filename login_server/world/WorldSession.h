#ifndef _WORLD_SESSION_H_
#define _WORLD_SESSION_H_

#include <functional>
#include <memory>
#include <queue>

#include "GameType.h"
#include "WorldDefine.h"
#include "WorldMsg.h"

class WorldSession : public std::enable_shared_from_this<WorldSession>
{
public:
    WorldSession(WorldConnection conn);
    ~WorldSession();

	void heartbeat(uint32_t diff);
    SESSION_STATE getSessionState() const;
    bool isClosed() const;

    //session被动关闭
    void closed();

    //session主动关闭
    void shutdown();

    const WorldConnection& getConnection() const;
    void postMsg(std::shared_ptr<WorldMsg> msg);

public:
    void reqPing(NetworkMsg& msg);
    void reqRtt(NetworkMsg& msg);
    void reqRegister(NetworkMsg& msg);
    void reqLogin(NetworkMsg& msg);

private:
    WorldConnection                       m_conn;
    SESSION_STATE                         m_state;
    std::deque<std::shared_ptr<WorldMsg>> m_world_msgs;
};

#endif
