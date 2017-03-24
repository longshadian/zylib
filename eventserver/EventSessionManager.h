#ifndef _event_SESSION_MANAGER_H_
#define _event_SESSION_MANAGER_H_

#include <map>

class EventSession;
struct EventSessionID;

class EventSessionManager
{
public:
                            EventSessionManager();
                            ~EventSessionManager();

    EventSession*           createSession(int fd);
    EventSession*           findSession(EventSessionID sid);
    void                    eraseSession(EventSessionID sid);
private:
    typedef std::map<EventSessionID, EventSession*> SessionMap;
    int32_t                 m_seq_id;
    SessionMap              m_sessions;
};

#endif