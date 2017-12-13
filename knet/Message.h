#pragma once

#include <vector>

#include "knet/KNetTypes.h"

namespace knet {

class MessageDecoder;
class MessageContext;
class RPCManager;

class KMessage
{
public:
    KMessage(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, RPCKey key);
    virtual ~KMessage() = default;
    KMessage(const KMessage&) = delete;
    KMessage& operator=(const KMessage&) = delete;
    KMessage(KMessage&&) = delete;
    KMessage& operator=(KMessage&&) = delete;

    bool                Invalid() const;
    const ServiceID&    GetFromSID() const;
    const ServiceID&    GetToSID() const;
    MsgID               GetMsgID() const;

    bool                HasKey() const;
    RPCKey              GetKey() const;
    const void*         GetKeyPtr() const;
    size_t              GetKeySize() const;

protected:
    ServiceID m_from_sid;
    ServiceID m_to_sid;
    MsgID     m_msg_id;
    RPCKey    m_key;
};

class SendMessage : public KMessage
{
    friend class MessageDecoder;
public:
    SendMessage(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, MsgType payload, RPCKey key);
    virtual ~SendMessage() = default;

    const void*         GetPtr() const;
    void*               GetPtr();
    size_t              GetSize() const;

private:
    std::vector<uint8_t> m_buffer;
    MsgType              m_payload;
};

class ReceivedMessage : public KMessage
{
    friend class MessageDecoder;
public:
    ReceivedMessage(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, RPCKey key);
    virtual ~ReceivedMessage();
    ReceivedMessage(const ReceivedMessage&) = delete;
    ReceivedMessage& operator=(const ReceivedMessage&) = delete;
    ReceivedMessage(ReceivedMessage&&) = delete;
    ReceivedMessage& operator=(ReceivedMessage&&) = delete;

    const MsgType& GetMsg() const;
private:
    MsgType              m_payload;
};

class MessageContext
{
public:
    MessageContext(RPCManager& rpc_mgr
        , ServiceID from_sid, ServiceID to_sid, const RPCKey& key);
    ~MessageContext() = default;

    void SendResponse(MsgID msg_id, MsgType msg);
    const ServiceID& GetFromSID() const;
    const ServiceID& GetToSID() const;
    const RPCKey& GetKey() const;

private:
    RPCManager& m_rpc_mgr;
    ServiceID   m_from_sid;
    ServiceID   m_to_sid;
    RPCKey      m_key;
};

struct MessageDecoder
{
    static void Encode(SendMessage& send_msg);
    static ReceivedMessagePtr Decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len);
};

} // knet
