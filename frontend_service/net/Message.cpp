#include "net/Message.h"

int32_t Message::GetMsgID() const
{
    return m_head.m_msg_id;
}

std::string Message::GetSID() const
{
    std::string s{};
    for (auto v : m_head.m_sid) {
        if (v == 0)
            break;;
        s.push_back(v);
    }
}

void ServerCallback::HandlerAccept(Hdl)
{

}

void ServerCallback::HandlerClosed(Hdl)
{

}

void ServerCallback::HandlerTimeout(Hdl)
{

}

void ServerCallback::ReceviedMessage(Hdl hdl, std::shared_ptr<Message> msg)
{
    (void)hdl;
    (void)msg;
}

void ServerCallback::HandlerAcceptOverflow()
{

}
