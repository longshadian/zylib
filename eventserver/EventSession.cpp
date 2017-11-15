#include "EventSession.h"

#include <cstring>
#include <vector>
#include <sstream>

#include <event2/buffer.h>

#include "EAssert.h"
#include "EventServer.h"
#include "EventBuffer.h"
#include "EventRequest.h"
#include "EventResponse.h"
#include "EventHandler.h"
#include "EventMessageDecoder.h"
#include "Log.h"

EventSessionID::EventSessionID(const EventSessionID& rhs)
{
    copy(rhs);
}

EventSessionID& EventSessionID::operator=(const EventSessionID& rhs)
{
    if (this != &rhs)
        copy(rhs);
    return *this;
}

bool EventSessionID::operator<(const EventSessionID& rhs) const
{
    if (m_fd == rhs.m_fd)
        return m_seq_id < rhs.m_seq_id;
    return m_fd < rhs.m_fd;
}

bool EventSessionID::operator==(const EventSessionID& rhs) const
{
    return m_fd == rhs.m_fd && m_seq_id == rhs.m_seq_id;
}

bool EventSessionID::operator!=(const EventSessionID& rhs) const
{
    return !(*this == rhs);
}

EventSessionID::operator bool() const
{
    return m_fd != 0 || m_seq_id != 0 ;
}

/*
std::ostream& EventSessionID::operator<<(std::ostream& os)
{
    os << m_fd << "." << m_seq_id;
    return os;
}
*/

void EventSessionID::copy(const EventSessionID& rhs)
{
    m_fd = rhs.m_fd;
    m_seq_id = rhs.m_seq_id;
}

std::string EventSessionID::toString() const
{
    std::ostringstream ostm{};
    ostm << "{" << m_fd << "." << m_seq_id << "}";
    return ostm.str();
}


//////////////////////////////////////////////////////////////////////////

EventSession::EventSession(EventSessionID sid)
    : m_shutdown(false)
    , m_wait_writen(false)
    , m_sid(sid)
    , m_event_server(NULL)
    , m_handler(NULL)
    , m_received_buffer(new EventBuffer())
    , m_send_buffer(new EventBuffer())
    , m_request(new EventRequest())
    , m_read_ev(NULL)
    , m_write_ev(NULL)
{
    m_request->setSessionID(m_sid);
}

EventSession::~EventSession()
{
    delete m_handler;
    delete m_received_buffer;
    delete m_send_buffer;
    delete m_request;

    if (m_read_ev)
        event_free(m_read_ev);
    if (m_write_ev)
        event_free(m_write_ev);
    evutil_closesocket(m_sid.m_fd);
}

EventRequest* EventSession::getRequest()
{
    return m_request;
}

void EventSession::setServer(EventServer* event_server)
{
    m_event_server = event_server;
}

void EventSession::setHandler(EventHandler* handler)
{
    m_handler = handler;
}

EventHandler* EventSession::getHandler()
{
    return m_handler;
}

void EventSession::appendResponse(EventResponse& resp)
{
    m_send_buffer->append(resp.getBuffer());
}

void EventSession::appendResponse(const void* data, int len)
{
    if (len <= 0)
        return;
    m_send_buffer->append(data, len);
}

void EventSession::addEvent(short ev)
{
    if (ev & EV_WRITE) {
        if (!m_write_ev)
            m_write_ev = event_new(m_event_server->getEventBase(), m_sid.m_fd, EV_WRITE, &EventSession::_onWrite, this);
        event_add(m_write_ev, NULL);
        m_wait_writen = true;
        return;
    }
    if (ev & EV_READ) {
        if (!m_read_ev)
            m_read_ev = event_new(m_event_server->getEventBase(), m_sid.m_fd, EV_READ, &EventSession::_onRead, this);

        struct timeval timeout;
        memset(&timeout, 0, sizeof(timeout));
        timeout.tv_sec = m_event_server->getTimeOut();
        event_add(m_read_ev, &timeout);
        return;
    }
}

void EventSession::shutdown(int ev)
{
    (void)ev;
    if (m_shutdown)
        return;
    m_shutdown = true;
    //PRINT_LOG_EX("test", LOG_ERROR, "xxx %d %d shutdown", m_sid.m_fd, m_sid.m_seq_id);
    m_event_server->shutdownSession(m_sid);
}

bool EventSession::isClosed() const
{
    return m_shutdown;
}

void EventSession::asyncWrite()
{
    if (m_wait_writen)
        return;
    onWrite(m_sid.m_fd, EV_WRITE);
}

bool EventSession::release()
{
    m_shutdown = true;
    //session还有write事件，暂时不关闭。在write回调结束后会再次检测能否关闭
    if (m_wait_writen)
        return false;
    if (m_read_ev)
        event_del(m_read_ev);
    if (m_write_ev)
        event_del(m_write_ev);
    return true;
}

void EventSession::_onRead(int fd, short ev, void* arg)
{
    EventSession* self = (EventSession*)arg;
    self->onRead(fd, ev);
}

void EventSession::_onWrite(int fd, short ev, void* arg)
{
    EventSession* self = (EventSession*)arg;
    self->onWrite(fd, ev);
}

void EventSession::onRead(int fd, short ev)
{
    if (isClosed())
        return;

    if (ev & EV_READ) {
        int read_len = m_received_buffer->read(fd);
        if (read_len > 0) {
            int msg_count = getRequest()->getMsgDecoder()->decode(m_received_buffer);
            if (msg_count > 0) {
                std::vector<EventMessage*> msgs;
                msgs.reserve(msg_count);
                for (int i = 0; i != msg_count; ++i) {
                    msgs.push_back(getRequest()->getMsgDecoder()->take());
                }
                getHandler()->handle(getRequest(), msgs);
            }

            if (msg_count >= 0) {
                addEvent(EV_READ);
            } else if (msg_count < 0) {
                getHandler()->close(getRequest());
                shutdown();
            }
        } else {
            getHandler()->close(getRequest());
            shutdown();
        }
    } else {
        getHandler()->timeout(getRequest());
        shutdown();
    }
}

void EventSession::onWrite(int fd, short ev)
{
    m_wait_writen = false;
    if (!(ev & EV_WRITE))
        return;

    int write_len = m_send_buffer->getSize();
    while (write_len > 0) {
        if ((write_len = m_send_buffer->write(fd)) <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                addEvent(EV_WRITE);
            } else {
                getHandler()->close(getRequest());
                shutdown();
            }
            break;
        }
        write_len = m_send_buffer->getSize();
    }

    //session write结束后检测该session是否已经处于关闭状态。
    //如果已经处于关闭状态，通知event_server关闭该server
    if (!m_wait_writen && isClosed()) {
        m_event_server->shutdownSession(m_sid);
    }
}