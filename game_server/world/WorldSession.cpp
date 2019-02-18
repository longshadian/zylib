#include "WorldSession.h"

#include "zylib/Random.h"

#include "Log.h"
#include "GlobalService.h"
#include "NetworkService.h"

WorldSession::WorldSession(WorldConnection conn)
	: m_conn(conn)
    , m_state(SESSION_STATE::UNLOGIN)
    , m_world_msgs()
{
}

WorldSession::~WorldSession()
{
}

SESSION_STATE WorldSession::getSessionState() const { return m_state; }

void WorldSession::heartbeat(uint32_t diff)
{
    (void)diff;
    while (!isClosed() && !m_world_msgs.empty()) {
        auto msg_cb = std::move(m_world_msgs.front());
        m_world_msgs.pop_front();
        if (msg_cb)
            msg_cb->visitor(this);
    }
}

bool WorldSession::isClosed() const
{
    return m_state == SESSION_STATE::CLOSED;
}

void WorldSession::shutdown()
{
    if (!isClosed()) {
        LOG(DEBUG) << "xxxxxxxxxxxxxxxxx shutdown";
    }
    closed();
}

void WorldSession::closed()
{
    m_state = SESSION_STATE::CLOSED;
}

const WorldConnection& WorldSession::getConnection() const
{
    return m_conn;
}

void WorldSession::postMsg(std::shared_ptr<WorldMsg> msg)
{
    m_world_msgs.push_back(std::move(msg));
}

void WorldSession::reqPing(NetworkMsg& msg)
{
    auto tnow = std::chrono::system_clock::now();
    auto tdelta = std::chrono::duration_cast<std::chrono::milliseconds>(tnow - msg.m_timestamp).count();

    LOG(DEBUG) << "reqPing cost:" << tdelta << "ms msg_id:" << msg.m_msg_id << ":" << msg.m_data;
    Global::getNetworkService().sendMessage(m_conn, 1, msg.m_data + " hahah");
}

void WorldSession::reqRtt(NetworkMsg& msg)
{
    (void)msg;
}

void WorldSession::reqRegister(NetworkMsg& msg)
{
    (void)msg;
}

void WorldSession::reqLogin(NetworkMsg& msg)
{
    (void)msg;
}
