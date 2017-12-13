#pragma once

#include <vector>

#include "knet/KNetTypes.h"

namespace knet {

class MessageDecoder;
class ReceivedMessageContext;
class RPCManager;

class KMessage
{
public:
    KMessage(ServiceID id, MsgID msg_id, RPCKey key);
    virtual ~KMessage() = default;
    KMessage(const KMessage&) = delete;
    KMessage& operator=(const KMessage&) = delete;
    KMessage(KMessage&&) = delete;
    KMessage& operator=(KMessage&&) = delete;

    const ServiceID&    GetServiceID() const;
    MsgID               GetMsgID() const;

    bool                HasRPCKey() const;
    RPCKey              GetKey() const;
    const void*         GetRPCKeyPtr() const;
    size_t              GetRPCKeySize() const;

protected:
    ServiceID m_service_id;
    MsgID     m_msg_id;
    RPCKey    m_key;
};

class SendMessage : public KMessage
{
    friend class MessageDecoder;
public:
    SendMessage(ServiceID sid, MsgID msg_id, MsgType payload, RPCKey key);
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
    ReceivedMessage(ServiceID id, MsgID msg_id, RPCKey key);
    virtual ~ReceivedMessage();
    ReceivedMessage(const ReceivedMessage&) = delete;
    ReceivedMessage& operator=(const ReceivedMessage&) = delete;
    ReceivedMessage(ReceivedMessage&&) = delete;
    ReceivedMessage& operator=(ReceivedMessage&&) = delete;

    const MsgType& GetMsgType() const;
private:
    MsgType              m_payload;
};

class ReceivedMessageContext
{
public:
    ReceivedMessageContext(RPCManager& rpc_mgr);
    ~ReceivedMessageContext() = default;

    void SendResponse(MsgID msg_id, MsgType msg);
private:
    RPCManager& m_rpc_mgr;
    ServiceID   m_sid;
    RPCKey      m_key;
};

struct MessageDecoder
{
    static void Encode(SendMessage& send_msg);
    static ReceivedMessagePtr Decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len);
};

} // knet
