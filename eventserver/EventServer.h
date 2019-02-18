#ifndef _EVENT_SERVER_H_
#define _EVENT_SERVER_H_

#include <map>
#include <mutex>
#include <string>
#include <deque>
#include <atomic>

#include "EventSession.h"

class EventSessionManager;
class EventHandlerFactory;
class EventResponse;
class EventTask;

class EventServer
{
public:
                            EventServer();
                            ~EventServer();

    void                    setParams(int max_clients, int timeout_second, EventHandlerFactory* fty);

    /**
    * ��ʼ��������
    */
    bool                    listen(const char* ip, int port);

    /**
    * ���µ��߳��п�ʼ����
    */
    bool                    start();

    /**
    * ��ǰ�����Ƿ�������
    */
    bool                    isRunning() const;

    /**
    * �رշ���
    */
    void                    shutdown();

    /**
    * ����Respone
    */
    void                    sendResponse(EventResponse* resp);
    void                    shutdownSession(EventSessionID sid);

    struct event_base*      getEventBase();
    EventSessionManager*    getEventSessionMgr();    
    int                     getTimeOut() const;
private:
    static void             _onAccept(int fd, short events, void * arg);
    static void             _onReadPipe(int fd, short events, void* arg);

    bool                    service();
    void                    onAccept(int fd, short events);
    void                    onReadPipe(int fd, short events);
    void                    executeTask();
    void                    closedFD(int fd);

    void                    submitTask(EventTask* task);
private:
    EventSessionManager*	    m_session_mgr;
    EventHandlerFactory*        m_handler_fty;
    struct event_base*			m_event_base;
    std::atomic<bool>		    m_is_running;
    int                         m_listen_fd;
    int							m_write_fd;
    int							m_read_fd;
    int							m_timeout_second;
    int							m_max_clients;
    struct event*               m_pipe_ev;
    struct event*               m_listen_ev;
    std::deque<EventTask*>      m_event_tasks;
    mutable std::mutex 			m_lock;
};

#endif