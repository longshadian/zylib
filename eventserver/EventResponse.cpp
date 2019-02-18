#include "EventResponse.h"
#include "EventBuffer.h"

EventResponse::EventResponse(EventSessionID sid)
    : m_buffer(new EventBuffer())
    , m_sid(sid)
{
}

EventResponse::~EventResponse()
{
    delete m_buffer;
}

EventBuffer* EventResponse::getBuffer()
{
    return m_buffer;
}

EventSessionID EventResponse::getSessionID() const
{
    return m_sid;
}

void EventResponse::setSessionID(EventSessionID sid)
{
    m_sid = sid;
}