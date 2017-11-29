#include "knet/Message.h"

#include <cstring>

namespace knet {

KMessage::KMessage()
    : m_msg_id()
    , m_payload()
    , m_key()
{
}

KMessage::~KMessage()
{
}

bool KMessage::HasRPCKey() const
{
    return m_key != 0;
}

void KMessage::PayloadParseFromBinary(const void* p, size_t p_len)
{
    if (p && p_len > 0) {
        const auto* up = reinterpret_cast<const uint8_t*>(p);
        m_payload.assign(up, up + p_len);
    }
}

bool KMessage::KeyParseFromBinary(const void* key, size_t key_len)
{
    if (key && key_len) {
        std::memcpy(&m_key, key, key_len);
    }
    return key_len == sizeof(RPCKey);
}



void MessageDecoder::encode(MessagePtr msg, std::vector<uint8_t>* out)
{
    (void)msg;
    (void)out;
}

MessagePtr MessageDecoder::decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len)
{
    (void)p;
    (void)len;
    (void)key;
    (void)key_len;
    return nullptr;
}

} // knet
