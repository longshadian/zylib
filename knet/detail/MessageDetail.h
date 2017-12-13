#pragma once

#include <vector>

#include "knet/KNetTypes.h"
#include "knet/Message.h"

namespace knet {

class MessageDecoder;
class MessageContext;
class RPCManager;

namespace detail {

class SendMsg : public Message
{
    friend class MessageDecoder;
public:
    SendMsg(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, MsgType payload, Key key);
    virtual ~SendMsg() = default;

    const void*         GetPtr() const;
    void*               GetPtr();
    size_t              GetSize() const;

private:
    std::vector<uint8_t> m_buffer;
    MsgType              m_payload;
};

struct MessageDecoder
{
    static void Encode(SendMsg& send_msg);
    static ReceivedMsgPtr Decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len);
};

} // detail
} // knet
