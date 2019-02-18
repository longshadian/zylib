#pragma once

#include <vector>

#include "knet/KNetTypes.h"

namespace knet {

class ReceivedMsgCtx;
class RPCManager;

class Message
{
public:
    Message(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, Key key);
    virtual ~Message() = default;
    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;
    Message(Message&&) = delete;
    Message& operator=(Message&&) = delete;

    bool                Invalid() const;
    const ServiceID&    GetFromSID() const;
    const ServiceID&    GetToSID() const;
    MsgID               GetMsgID() const;
    Key                 GetKey() const;
    const void*         GetKeyPtr() const;
    size_t              GetKeySize() const;

protected:
    ServiceID           m_from_sid;
    ServiceID           m_to_sid;
    MsgID               m_msg_id;
    Key                 m_key;
};

class ReceivedMsg : public Message
{
public:
    ReceivedMsg(ServiceID from_sid, ServiceID to_sid, MsgID msg_id, MsgType payload, Key key);
    virtual ~ReceivedMsg();
    ReceivedMsg(const ReceivedMsg&) = delete;
    ReceivedMsg& operator=(const ReceivedMsg&) = delete;
    ReceivedMsg(ReceivedMsg&&) = delete;
    ReceivedMsg& operator=(ReceivedMsg&&) = delete;

    const MsgType&      GetMsg() const;
private:
    MsgType             m_payload;
};

class ReceivedMsgCtx
{
public:
    ReceivedMsgCtx(RPCManager& rpc_mgr, ServiceID from_sid, ServiceID to_sid, const Key& key);
    ~ReceivedMsgCtx() = default;
    ReceivedMsgCtx(const ReceivedMsgCtx& rhs) = delete;
    ReceivedMsgCtx& operator=(const ReceivedMsgCtx& rhs) = delete;
    ReceivedMsgCtx(ReceivedMsgCtx&& rhs) = delete;
    ReceivedMsgCtx& operator=(ReceivedMsgCtx&& rhs) = delete;

    void                SendResponse(MsgID msg_id, MsgType msg);
    const ServiceID&    GetFromSID() const;
    const ServiceID&    GetToSID() const;
    const Key&          GetKey() const;

private:
    RPCManager&         m_rpc_mgr;
    ServiceID           m_from_sid;
    ServiceID           m_to_sid;
    Key                 m_key;
};

} // knet
