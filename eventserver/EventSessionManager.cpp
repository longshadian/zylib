#include "EventSessionManager.h"

#include "EventSession.h"

EventSessionManager::EventSessionManager()
    : m_seq_id(0)
    , m_sessions()
{
}

EventSessionManager::~EventSessionManager()
{
    for (auto it : m_sessions) {
        delete it.second;
    }
}

EventSession* EventSessionManager::createSession(int fd)
{
    int seq_id = 0;
    if (m_seq_id == INT32_MAX)
        m_seq_id = 0;
    seq_id = ++m_seq_id;

    EventSessionID sid = {fd, seq_id};
    EventSession* session = new EventSession(sid);
    m_sessions.insert({sid, session});
    return session;
}

EventSession* EventSessionManager::findSession(EventSessionID sid)
{
    SessionMap::iterator it = m_sessions.find(sid);
    if (it != m_sessions.end())
        return it->second;
    return NULL;
}

void EventSessionManager::eraseSession(EventSessionID sid)
{
    m_sessions.erase(sid);
}

size_t EventSessionManager::sessionCount() const
{
    return m_sessions.size();
}
