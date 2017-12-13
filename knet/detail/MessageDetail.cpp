#include "knet/detail/MessageDetail.h"

#include <cstring>
#include <array>

#include "knet/FakeLog.h"

namespace knet {
namespace detail {

SendMsg::SendMsg(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, MsgType payload, Key key)
    : Message(std::move(from_sid), std::move(to_sid), std::move(msg_id), std::move(key))
    , m_payload(std::move(payload))
{
}

const void* SendMsg::GetPtr() const 
{ 
    return m_buffer.data();
}

void* SendMsg::GetPtr() 
{ 
    return m_buffer.data();
}

size_t SendMsg::GetSize() const 
{ 
    return m_buffer.size();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void MessageDecoder::Encode(SendMsg& send_msg)
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

ReceivedMsgPtr MessageDecoder::Decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len)
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
    Key rpc_key{reinterpret_cast<const char*>(key), reinterpret_cast<const char*>(key) + key_len};

    MsgType payload{pos, p + len};
    auto received_msg = std::make_shared<ReceivedMsg>(std::move(from_sid), std::move(to_sid)
        , msg_id , std::move(payload), rpc_key);
    return received_msg;
}

} // detail
} // knet
