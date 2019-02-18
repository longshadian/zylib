#ifndef _EVENT_MESSAGE_DECODER_H_
#define _EVENT_MESSAGE_DECODER_H_


#include <deque>

class EventBuffer;

class EventMessage
{
public:
    virtual                     ~EventMessage() {}
};

class EventMsgDecoder
{
public:
    virtual                     ~EventMsgDecoder() {}

    //return -1:解包失败，断开链接
    virtual int                 decode(EventBuffer* pBuf) = 0;

    int                         size() const;
    EventMessage*               take();
protected:
    void                        putMsg(EventMessage* pMsg);
protected:
    std::deque<EventMessage*>   m_msg_queue;
};

#endif