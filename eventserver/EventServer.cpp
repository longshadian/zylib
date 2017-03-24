#include "EventServer.h"

#include <fcntl.h>
#ifdef WIN32
#include <WinSock2.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <cstdio>
#include <cstring>

#include <event2/util.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

#include "EventSessionManager.h"
#include "EventSession.h"
#include "EventRequest.h"
#include "EventHandler.h"
#include "EventResponse.h"
#include "EventTask.h"

EventServer::EventServer()
    : m_session_mgr(NULL)
    , m_handler_fty(NULL)
    , m_event_base(NULL)
    , m_is_running(false)
    , m_listen_fd(-1)
    , m_write_fd(-1)
    , m_read_fd(-1)
    , m_timeout_second(0)
    , m_max_clients(0)
    , m_pipe_ev(NULL)
    , m_listen_ev(NULL)
    , m_event_tasks()
    , m_lock()
{
    m_event_base        = event_base_new();
    m_timeout_second    = 60;
    m_max_clients       = 1000000;
    m_session_mgr       = new EventSessionManager();
}

EventServer::~EventServer()
{
    if (m_session_mgr)
        delete m_session_mgr;
    if (m_handler_fty)
        delete m_handler_fty;
    while (!m_event_tasks.empty()) {
        EventTask* task = m_event_tasks.front();
        delete task;
        m_event_tasks.pop_front();
    }
    event_base_free(m_event_base);
}

void EventServer::setParams(int max_clients, int timeout_second, EventHandlerFactory* fty)
{
    m_max_clients   = max_clients;
    m_timeout_second       = timeout_second;
    m_handler_fty   = fty;
}

bool EventServer::listen(const char* ip, int port)
{
    (void)ip;
    if (m_is_running || 0 <= m_listen_fd || port <= 0)
        return false;

#ifdef WIN32
    {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(1, 1), &wsaData);
    }
#endif

    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0) {
        return false;
    }

    if (evutil_make_socket_nonblocking(m_listen_fd) != 0) {
        closedFD(m_listen_fd);
        return false;
    }
    if (evutil_make_listen_socket_reuseable(m_listen_fd) != 0) {
        closedFD(m_listen_fd);
        return false;
    }

    /*
        int flags = 1;
        if (setsockopt(m_listen_fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&flags, sizeof(flags)) < 0) {
            return false;
        }
    */

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<short>(port));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        closedFD(m_listen_fd);
        return false;
    }
    if (::listen(m_listen_fd, 5) < 0) {
        closedFD(m_listen_fd);
        return false;
    }
    return true;
}

bool EventServer::start()
{
    if (m_is_running)
        return false;
    m_is_running = true;
    return service();
}

bool EventServer::isRunning() const
{
    return m_is_running;
}

void EventServer::shutdown()
{
    if (!m_is_running.exchange(false))
        return;

    std::lock_guard<std::mutex> lk(m_lock);
    event_base_loopexit(m_event_base, NULL);
    char cmd = 1;
    send(m_write_fd, &cmd, sizeof(char), 0);
    //event_base_loopbreak(m_event_base);
}

void EventServer::sendResponse(EventResponse* resp)
{
    submitTask(new EventSendTask(*this, resp));
}

void EventServer::shutdownSession(EventSessionID sid)
{
    submitTask(new EventShutdownSessionTask(*this, sid));
}

struct event_base* EventServer::getEventBase()
{
    return m_event_base;
}

EventSessionManager* EventServer::getEventSessionMgr()
{
    return m_session_mgr;
}

int EventServer::getTimeOut() const
{
    return m_timeout_second;
}

void EventServer::_onAccept(int fd, short events, void * arg)
{
    EventServer* p_this = (EventServer*)arg;
    p_this->onAccept(fd, events);
}

void EventServer::onAccept(int fd, short events)
{
    (void)events;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept");
        return;
    }

    if (evutil_make_socket_nonblocking(client_fd) != 0) {
        perror("accept");
        closedFD(client_fd);
        return;
    }

    EventSession* session = m_session_mgr->createSession(client_fd);
    if (!session) {
        perror("createSession");
        closedFD(client_fd);
        return;
    }

    /*
    char client_ip[32] = { 0 };
    const unsigned char *pos = (const unsigned char *)&client_addr.sin_addr;
    snprintf(client_ip, sizeof(client_ip), "%i.%i.%i.%i", pos[0], pos[1], pos[2], pos[3]);
    session->getRequest()->setClientIP(client_ip);
    */
    std::string s = inet_ntoa(client_addr.sin_addr);
    if (s.empty())
        s = "unknown";
    session->getRequest()->setClientIP(s.c_str());

    session->setServer(this);
    session->setHandler(m_handler_fty->create());

    session->getHandler()->start(session->getRequest());
    session->addEvent(EV_READ);
}

void EventServer::_onReadPipe(int fd, short events, void* arg)
{
    EventServer* p_this = (EventServer*)arg;
    p_this->onReadPipe(fd, events);
}

void EventServer::onReadPipe(int fd, short events)
{
    if (events & EV_READ) {
        char cmd[1024] = {0};
        recv(fd, cmd, sizeof(cmd), 0);
        //::read(fd, &cmd, sizeof(char));
        executeTask();
    }
}

void EventServer::executeTask()
{
    EventTask* task = NULL;
    while (true) {
        {
            std::lock_guard<std::mutex> lk(m_lock);
            if (m_event_tasks.empty())
                return;
            task = m_event_tasks.front();
            m_event_tasks.pop_front();
        }
        if (task)
        task->run();
        delete task;
        task = NULL;
    }
}

bool EventServer::service()
{
    if (m_listen_fd < 0)
        return false;

    int pipe_fds[2];
    if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pipe_fds) != 0)
        return false;

    m_write_fd = pipe_fds[0];
    if (evutil_make_socket_nonblocking(m_write_fd) != 0)
        return false;

    m_read_fd = pipe_fds[1];

    m_pipe_ev = event_new(m_event_base, m_read_fd, EV_READ | EV_PERSIST, &EventServer::_onReadPipe, this);
    event_add(m_pipe_ev, NULL);

    m_listen_ev = event_new(m_event_base, m_listen_fd, EV_READ | EV_PERSIST, &EventServer::_onAccept, this);
    event_add(m_listen_ev, NULL);
    event_base_dispatch(m_event_base);

    event_del(m_pipe_ev);
    event_del(m_listen_ev);
    return true;
}

void EventServer::closedFD(int fd)
{
    evutil_closesocket(fd);
}

void EventServer::submitTask(EventTask* task)
{
    std::lock_guard<std::mutex> lk(m_lock);
    m_event_tasks.push_back(task);
    char cmd = 1;
    send(m_write_fd, &cmd, sizeof(char), 0);
}