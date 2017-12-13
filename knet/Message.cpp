#include "knet/Message.h"

#include <cstring>
#include <array>

#include "knet/FakeLog.h"
#include "knet/RPCManager.h"

namespace knet {

KMessage::KMessage(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, RPCKey key)
    : m_from_sid(std::move(from_sid))
    , m_to_sid(std::move(to_sid))
    , m_msg_id(msg_id)
    , m_key(key)
{
}

bool KMessage::Invalid() const
{
    return m_from_sid.empty() || m_to_sid.empty();
}

const ServiceID& KMessage::GetFromSID() const
{
    return m_from_sid;
}

const ServiceID& KMessage::GetToSID() const
{
    return m_to_sid;
}

MsgID KMessage::GetMsgID() const
{
    return m_msg_id;
}

bool KMessage::HasKey() const
{
    return true;
}

RPCKey KMessage::GetKey() const
{
    return m_key;
}

const void* KMessage::GetKeyPtr() const 
{ 
    return m_key.data();
}

size_t KMessage::GetKeySize() const 
{ 
    return m_key.size();
}

SendMessage::SendMessage(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, MsgType payload, RPCKey key)
    : KMessage(std::move(from_sid), std::move(to_sid), std::move(msg_id), std::move(key))
    , m_payload(std::move(payload))
{
}

const void* SendMessage::GetPtr() const 
{ 
    return m_buffer.data();
}

void* SendMessage::GetPtr() 
{ 
    return m_buffer.data();
}

size_t SendMessage::GetSize() const 
{ 
    return m_buffer.size();
}


ReceivedMessage::ReceivedMessage(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, RPCKey key)
    : KMessage(std::move(from_sid), std::move(to_sid), msg_id, key)
{
}

ReceivedMessage::~ReceivedMessage()
{
}

const MsgType& ReceivedMessage::GetMsg() const
{
    return m_payload;
}

MessageContext::MessageContext(RPCManager& rpc_mgr
    , ServiceID from_sid, ServiceID to_sid, const RPCKey& key) 
    : m_rpc_mgr(rpc_mgr)
    , m_from_sid(std::move(from_sid))
    , m_to_sid(std::move(to_sid))
    , m_key(key)
{

}

void MessageContext::SendResponse(MsgID msg_id, MsgType msg)
{
    m_rpc_mgr.RPCResponse(*this, msg_id, std::move(msg));
}

const ServiceID& MessageContext::GetFromSID() const
{
    return m_from_sid;
}

const ServiceID& MessageContext::GetToSID() const
{
    return m_to_sid;
}

const RPCKey& MessageContext::GetKey() const
{
    return m_key;
}

void MessageDecoder::Encode(SendMessage& send_msg)
{
    const auto& from_sid = send_msg.GetFromSID();
    const auto& to_sid = send_msg.GetToSID();
    if (from_sid.empty() || to_sid.empty())
        return;
    auto& buffer = send_msg.m_buffer;
    buffer.resize(from_sid.size() + 1 + to_sid.size() + 1 + sizeof(MsgID) + send_msg.m_payload.size());
    auto* pos = buffer.data();

    std::memcpy(pos, from_sid.data(), from_sid.size());
    pos += from_sid.size() + 1;

    std::memcpy(pos, to_sid.data(), to_sid.size());
    pos += to_sid.size() + 1;

    auto msg_id = send_msg.GetMsgID();
    std::memcpy(pos, &msg_id, sizeof(msg_id));
    pos += sizeof(msg_id);

    std::memcpy(pos, send_msg.m_payload.data(), send_msg.m_payload.size());
}

ReceivedMessagePtr MessageDecoder::Decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len)
{
    if (len <= 4)
        return nullptr;
    if (!key || key_len == 0)
        return nullptr;

    const uint8_t* p_from = nullptr;
    const uint8_t* p_to = nullptr;
    for (size_t i = 0; i != len; ++i) {
        if (p[i] == 0) {
            if (!p_from) {
                p_from = p + i;
                continue;
            }
            if (!p_to) {
                p_to = p + i;
                break;
            }
        }
    }
    if (!p_from || !p_to) {
        FAKE_LOG(WARNING) << " p_from: " << p_from << " p_to: " << p_to;
        return nullptr;
    }

    const auto* pos = p;
    ServiceID from_sid{pos, p_from};
    if (from_sid.empty()) {
        FAKE_LOG(WARNING) << " from_sid empty ";
        return nullptr;
    }

    pos = p_from + 1;
    ServiceID to_sid{pos, p_to};
    if (to_sid.empty()) {
        FAKE_LOG(WARNING) << " to_sid empty ";
        return nullptr;
    }

    pos = p_to + 1;
    if (pos + sizeof(MsgID) > p + len) {
        FAKE_LOG(WARNING) << " msg_id error ";
        return nullptr;
    }
    MsgID msg_id{};
    std::memcpy(&msg_id, pos, sizeof(MsgID));

    pos += sizeof(MsgID);
    RPCKey rpc_key{reinterpret_cast<const char*>(key), reinterpret_cast<const char*>(key) + key_len};
    auto received_msg = std::make_shared<ReceivedMessage>(std::move(from_sid), std::move(to_sid), msg_id, rpc_key);
    received_msg->m_payload.append(pos, p + len);
    return received_msg;
}

} // knet
