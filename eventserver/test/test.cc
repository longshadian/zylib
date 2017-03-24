
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"

#ifdef WIN32
#include <winsock.h>
#endif

#include <iostream>
#include <string>

#include "EventServer.h"
#include "EventMessageDecoder.h"
#include "EventHandler.h"
#include "EventBuffer.h"
#include "EventRequest.h"
#include "EventResponse.h"

class TestMessage : public EventMessage
{
public:
   TestMessage() {}
   virtual ~TestMessage() {}
    std::vector<char> m_data;
};

class TestDecoder : public EventMsgDecoder
{
public:
                        TestDecoder() {}
    virtual             ~TestDecoder() {}

    virtual int         decode(EventBuffer* pBuf) override
    {
        int total = pBuf->getSize();
        if (total < 0)
            return -1;
        if (total == 0)
            return 0;

        TestMessage* msg = new TestMessage();
        msg->m_data.resize(total);
        pBuf->take(&msg->m_data[0], msg->m_data.size());
        putMsg(msg);
        return size();
    }
};

class TestHandler : public EventHandler
{
    EventServer&    m_server;
public:
    TestHandler(EventServer& s) : m_server(s) {}
    virtual                     ~TestHandler() {}
    virtual int                 start(EventRequest* req) override
    {
        req->setMsgDecoder(new TestDecoder());
        std::cout << "ip:" << req->getClientIP() << "\t" << req->getSessionID() << std::endl;
        return 0;
    }

    virtual int                 handle(EventRequest* req, std::vector<EventMessage*> msgs) override
    {
        for (auto it : msgs) {
            TestMessage* msg = (TestMessage*)it;

            EventResponse* resp = new EventResponse(req->getSessionID());
            resp->getBuffer()->append(msg->m_data.data(), msg->m_data.size());
            resp->getBuffer()->append("xxxxxx");
            m_server.sendResponse(resp);
            delete msg;
            return 0;
        }
        return 0;
    }

    virtual void                close(EventRequest* req) override
    {
        std::cout << "close:" << req->getSessionID() << std::endl;
    }

    virtual void                timeout(EventRequest* req) override
    {
        std::cout << "timeout:" << req->getSessionID() << std::endl;
    }
};

class TestHandlerFty : public EventHandlerFactory
{
public:
    TestHandlerFty(EventServer& s) : m_server(s) {}
    virtual ~TestHandlerFty() {}

    virtual EventHandler*       create() override
    {
        return new TestHandler(m_server);
    }
private:
    EventServer& m_server;
};

int main()
{
    EventServer server;
    if (!server.listen(NULL, 22222))
        std::cout << "error listen" << std::endl;
    server.setParams(10, 100, new TestHandlerFty(server));
    if (!server.start())
        std::cout << "error listen" << std::endl;

    std::cout << "server shutdwon" << std::endl;

    system("pause");
    return 0;
}