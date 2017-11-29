#include "knet/Message.h"

#include <cstring>
#include <array>

namespace knet {

KMessage::KMessage(ServiceID id, MsgID msg_id, RPCKey key)
    : m_service_id(std::move(id))
    , m_msg_id(msg_id)
    , m_key(key)
{
}

const ServiceID& KMessage::GetServiceID() const 
{ 
    return m_service_id; 
}

MsgID KMessage::GetMsgID() const
{
    return m_msg_id;
}

bool KMessage::HasRPCKey() const
{
    return m_key != 0;
}

RPCKey KMessage::GetKey() const
{
    return m_key;
}

const void* KMessage::GetRPCKeyPtr() const 
{ 
    return m_key == 0 ? nullptr : &m_key; 
}

size_t KMessage::GetRPCKeySize() const 
{ 
    return GetRPCKeyPtr() ? sizeof(m_key) : 0; 
}


SendMessage::SendMessage(ServiceID sid, MsgID msg_id, MsgType payload, RPCKey key)
    : KMessage(std::move(sid), std::move(msg_id), std::move(key))
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


ReceivedMessage::ReceivedMessage(ServiceID id, MsgID msg_id, RPCKey key)
    : KMessage(std::move(id), msg_id, key)
{
}

ReceivedMessage::~ReceivedMessage()
{
}

void MessageDecoder::encode(SendMessage& send_msg)
{
    auto& buffer = send_msg.m_buffer;
    buffer.resize(8 + 4 + send_msg.m_payload.size());
    auto* p = buffer.data();

    std::array<uint8_t, 8> topic_name{};
    std::memcpy(topic_name.data(), send_msg.GetServiceID().data(), send_msg.GetServiceID().size());
    std::memcpy(p, topic_name.data(), topic_name.size());
    p += topic_name.size();

    auto msg_id = send_msg.GetMsgID();
    std::memcpy(p, &msg_id, sizeof(msg_id));
    p += sizeof(msg_id);
    std::memcpy(p, send_msg.m_payload.data(), send_msg.m_payload.size());
}

ReceivedMessagePtr MessageDecoder::decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len)
{
    if (len < 8 + 4)
        return nullptr;
    if (key_len != 0 && key_len != 8) {
        return nullptr;
    }

    std::array<char, 8> topic_name{};
    std::memcpy(topic_name.data(), p, 8);
    ServiceID sid{};
    sid = topic_name.data();
    p += 8;

    MsgID msg_id{};
    std::memcpy(&msg_id, p, sizeof(MsgID));
    p += 4;

    RPCKey rpc_key{};
    if (key && key_len != 0) {
        std::memcpy(&rpc_key, key, 8);
    }

    auto received_msg = std::make_shared<ReceivedMessage>(std::move(sid), msg_id, rpc_key);
    received_msg->m_payload.append(p, p + len - 8 - 4);
    return received_msg;
}

} // knet
