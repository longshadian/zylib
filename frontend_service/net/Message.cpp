#include "net/Message.h"

#include <cstring>
#include <array>

int32_t CSMessage::GetMsgID() const
{
    return m_head.m_msg_id;
}

std::string CSMessage::GetSID() const
{
    std::array<char, cs::SID_LENTH + 1> arr{};
    std::memcpy(arr.data(), m_head.m_sid.data(), m_head.m_sid.size());
    return std::string(arr.data());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

std::shared_ptr<CSMessage> CSDecode(const uint8_t* data, size_t len)
{
    // TODO 检验头部是否合法
    if (len < cs::HEAD_LENGTH)
        return nullptr;
    auto msg = std::make_shared<cs::CSMessage>();
    const auto* p = data;
    std::memcpy(&msg->m_head, p, sizeof(msg->m_head));
    p += sizeof(msg->m_head);
    msg->m_body.assign(p, data + len);
    return msg;
}

bool CSEncode()
{
    // TODO
    return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void ServerCallback::HandlerAccept(Hdl)
{
}

void ServerCallback::HandlerClosed(Hdl)
{

}

void ServerCallback::HandlerTimeout(Hdl)
{

}

void ServerCallback::ReceviedMessage(Hdl, std::shared_ptr<CSMessage>)
{

}

void ServerCallback::HandlerAcceptOverflow()
{
    // TODO;
}
