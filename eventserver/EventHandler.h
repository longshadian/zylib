#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include <vector>

class EventRequest;
class EventResponse;
class EventMessage;

class EventHandler
{
public:
    virtual                     ~EventHandler() {}
    virtual int                 start(EventRequest* req) = 0;
    virtual int                 handle(EventRequest* req, std::vector<EventMessage*> msg) = 0;
    virtual void                close(EventRequest* req) = 0;
    virtual void                timeout(EventRequest* req) = 0;
};

class EventHandlerFactory
{
public:
    virtual                     ~EventHandlerFactory() {};
    virtual EventHandler*       create() = 0;
};

#endif