#include "knet/Message.h"

#include <cstring>
#include <array>

#include "knet/FakeLog.h"
#include "knet/RPCManager.h"

namespace knet {

Message::Message(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, Key key)
    : m_from_sid(std::move(from_sid))
    , m_to_sid(std::move(to_sid))
    , m_msg_id(msg_id)
    , m_key(key)
{
}

bool Message::Invalid() const
{
    return m_from_sid.empty() || m_to_sid.empty();
}

const ServiceID& Message::GetFromSID() const
{
    return m_from_sid;
}

const ServiceID& Message::GetToSID() const
{
    return m_to_sid;
}

MsgID Message::GetMsgID() const
{
    return m_msg_id;
}

Key Message::GetKey() const
{
    return m_key;
}

const void* Message::GetKeyPtr() const 
{ 
    return m_key.data();
}

size_t Message::GetKeySize() const 
{ 
    return m_key.size();
}

ReceivedMsg::ReceivedMsg(ServiceID from_sid
    , ServiceID to_sid
    , MsgID msg_id
    , MsgType payload
    , Key key)
    : Message(std::move(from_sid), std::move(to_sid), msg_id, key)
    , m_payload(std::move(payload))
{
}

ReceivedMsg::~ReceivedMsg()
{
}

const MsgType& ReceivedMsg::GetMsg() const
{
    return m_payload;
}

ReceivedMsgCtx::ReceivedMsgCtx(RPCManager& rpc_mgr
    , ServiceID from_sid, ServiceID to_sid, const Key& key) 
    : m_rpc_mgr(rpc_mgr)
    , m_from_sid(std::move(from_sid))
    , m_to_sid(std::move(to_sid))
    , m_key(key)
{

}

void ReceivedMsgCtx::SendResponse(MsgID msg_id, MsgType msg)
{
    m_rpc_mgr.RPCResponse(*this, msg_id, std::move(msg));
}

const ServiceID& ReceivedMsgCtx::GetFromSID() const
{
    return m_from_sid;
}

const ServiceID& ReceivedMsgCtx::GetToSID() const
{
    return m_to_sid;
}

const Key& ReceivedMsgCtx::GetKey() const
{
    return m_key;
}

} // knet
