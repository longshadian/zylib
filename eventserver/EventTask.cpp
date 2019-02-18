#include "EventTask.h"

#include "EventServer.h"
#include "EventSessionManager.h"
#include "EventSession.h"
#include "EventResponse.h"
#include "EAssert.h"
#include "Log.h"

EventSendTask::EventSendTask(EventServer& server, EventResponse* resp)
    : m_server(server), m_resp(resp)
{
}

EventSendTask::~EventSendTask()
{
    if (m_resp)
        delete m_resp;
}

void EventSendTask::run()
{
    EventSession* session = m_server.getEventSessionMgr()->findSession(m_resp->getSessionID());
    if (!session || session->isClosed()) {
        return;
    } else {
        session->appendResponse(*m_resp);
        session->asyncWrite();
    }
}

//////////////////////////////////////////////////////////////////////////

EventShutdownSessionTask::EventShutdownSessionTask(EventServer& server, EventSessionID sid)
    : m_server(server), m_sid(sid)
{
}

EventShutdownSessionTask::~EventShutdownSessionTask()
{
}

void EventShutdownSessionTask::run()
{
    EventSession* session = m_server.getEventSessionMgr()->findSession(m_sid);
    if (!session) {
        return;
    }

    if (session->release()) {
        m_server.getEventSessionMgr()->eraseSession(m_sid);
        delete session;
        session = NULL;
    } else {
        //session����write�¼�,��ʱ�����ͷš�
        //write�¼�������event_server���ٴ��ͷ�session
    }
}
