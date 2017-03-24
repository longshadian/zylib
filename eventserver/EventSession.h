#ifndef _EVENT_SESSION_H_
#define _EVENT_SESSION_H_

#include <ostream>

#include <event2/event.h>

class EventRequest;
class EventServer;
class EventHandler;
class EventBuffer;
class EventResponse;

struct EventSessionID
{
                    EventSessionID(int fd = 0, int seq_id = 0) : m_fd(fd), m_seq_id(seq_id) {}
                    EventSessionID(const EventSessionID& rhs);
    EventSessionID& operator=(const EventSessionID& rhs);
    bool            operator<(const EventSessionID& rhs) const; 
    bool            operator==(const EventSessionID& rhs) const; 
    bool            operator!=(const EventSessionID& rhs) const; 
    operator        bool() const;

    int             m_fd;
    int             m_seq_id;
private:
    void            copy(const EventSessionID& rhs);
};

inline
std::ostream&   operator<<(std::ostream& os, EventSessionID sid)
{
    os << sid.m_fd << "." << sid.m_seq_id;
    return os;
}

class EventSession
{
public:
    enum
    {
        SHUTDOWN_READ   = 1,
        SHUTDOWN_WRITE  = 2,
    };
                            EventSession(EventSessionID sid);
                            ~EventSession();

    EventRequest*           getRequest();
    void                    setServer(EventServer* event_server);
    void                    setHandler(EventHandler* handler);
    EventHandler*           getHandler();

	void                    appendResponse(EventResponse& resp);
    void                    appendResponse(const void* data, int len);
	void                    addEvent(short ev);
	void                    shutdown(int ev = SHUTDOWN_READ | SHUTDOWN_WRITE);
    bool                    isClosed() const;
    void                    asyncWrite();
    bool                    release();
private:
    static void             _onRead(int fd, short ev, void* arg);
    static void             _onWrite(int fd, short ev, void* arg);

    void                    onRead(int fd, short ev);
    void                    onWrite(int fd, short ev);
private:
    bool                    m_shutdown;
    bool                    m_wait_writen;
    EventSessionID          m_sid;
    EventServer*            m_event_server;
    EventHandler*           m_handler;
    EventBuffer*            m_received_buffer;
    EventBuffer*            m_send_buffer;
    EventRequest*           m_request;
    struct event*	        m_read_ev;
    struct event*	        m_write_ev;
};

#endif