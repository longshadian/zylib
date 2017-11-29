#pragma once

#include <vector>

#include "knet/KNetTypes.h"

namespace knet {

class KMessage
{
public:
    KMessage();
    virtual ~KMessage();
    KMessage(const KMessage&) = delete;
    KMessage& operator=(const KMessage&) = delete;
    KMessage(KMessage&&) = delete;
    KMessage& operator=(KMessage&&) = delete;

    bool HasRPCKey() const;
    void PayloadParseFromBinary(const void* p, size_t p_len);
    bool KeyParseFromBinary(const void* key, size_t key_len);

    int32_t              m_msg_id;
    std::vector<uint8_t> m_payload;
    RPCKey               m_key;
};

class KMessageContext
{
public:
    KMessageContext();
    virtual ~KMessageContext();

};

struct MessageDecoder
{
    static void encode(MessagePtr msg, std::vector<uint8_t>* out);
    static MessagePtr decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len);
};

} // knet
