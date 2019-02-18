#ifndef _EVENT_TASK_H_
#define _EVENT_TASK_H_

#include "EventSession.h"

class EventResponse;
class EventServer;


class EventTask
{
public:
    virtual         ~EventTask() {}
    virtual void    run() = 0;
};

class EventSendTask : public EventTask
{
public:
                    EventSendTask(EventServer& server, EventResponse* resp);
    virtual         ~EventSendTask();
    virtual void    run();
private:
    EventServer&    m_server;
    EventResponse*  m_resp;
};

class EventShutdownSessionTask : public EventTask
{
public:
                    EventShutdownSessionTask(EventServer& server, EventSessionID sid);
    virtual         ~EventShutdownSessionTask();
    virtual void    run();
private:
    EventServer&    m_server;
    EventSessionID  m_sid;
};

#endif