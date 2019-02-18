#include "EventMessageDecoder.h"

int EventMsgDecoder::size() const
{
    return (int)m_msg_queue.size();
}

EventMessage* EventMsgDecoder::take()
{
    if (m_msg_queue.empty())
        return nullptr;
    EventMessage* msg = m_msg_queue.front();
    m_msg_queue.pop_front();
    return msg;
}

void EventMsgDecoder::putMsg(EventMessage* pMsg)
{
    m_msg_queue.push_back(pMsg);
}