#include "EventRequest.h"
#include "EventMessageDecoder.h"

EventRequest::EventRequest()
    : m_sid()
    , m_client_ip()
    , m_decoder(nullptr)
{
}

EventRequest::~EventRequest()
{
    if (m_decoder)
        delete m_decoder;
}

EventSessionID EventRequest::getSessionID() const
{
    return m_sid;
}

void EventRequest::setSessionID(EventSessionID sid)
{
    m_sid = sid;
}

void EventRequest::setClientIP(const char* ip)
{
    if (ip)
        m_client_ip = ip;
}
std::string EventRequest::getClientIP() const
{
    return m_client_ip;
}

void EventRequest::setMsgDecoder(EventMsgDecoder* decoder)
{
    if (m_decoder) {
        delete m_decoder;
        m_decoder = NULL;
    }
    m_decoder = decoder;
}

EventMsgDecoder* EventRequest::getMsgDecoder()
{
    return m_decoder;
}