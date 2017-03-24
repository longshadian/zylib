#ifndef _EVENT_RESPONSE_H_
#define _EVENT_RESPONSE_H_

#include "EventSession.h"

class EventBuffer;

class EventResponse
{
public:
                        EventResponse(EventSessionID sid);
                        ~EventResponse();

    EventBuffer*        getBuffer();
    EventSessionID      getSessionID() const;
    void                setSessionID(EventSessionID sid);
private:
    EventBuffer*        m_buffer;
    EventSessionID      m_sid;
};

#endif