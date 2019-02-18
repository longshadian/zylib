#ifndef _EVENT_REQUEST_H_
#define _EVENT_REQUEST_H_

#include <string>

#include "EventSession.h"

class EventMsgDecoder;

class EventRequest
{
public:
                        EventRequest();
                        ~EventRequest();

    EventSessionID      getSessionID() const;
    void                setSessionID(EventSessionID sid);
    void                setClientIP(const char* ip);
    std::string         getClientIP() const;
    void                setMsgDecoder(EventMsgDecoder* decoder);
    EventMsgDecoder*    getMsgDecoder();
private:
    EventSessionID      m_sid;
    std::string         m_client_ip;
    EventMsgDecoder*    m_decoder;
};

#endif