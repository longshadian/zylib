#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <queue>
#include <mutex>

#include "knet/KNetTypes.h"

namespace knet {

namespace detail {

class Producer;
class Consumer;

}

class CallbackManager;
class TimerManager;
class UniformNetwork;
class RPCManager;
class MessageContext;

using RPCSuccessCB = std::function<void(ReceivedMessagePtr msg)>;
using RPCTimeoutCB = std::function<void()>;

class RPCContext
{
    friend class RPCManager;
public:
    RPCContext() = default;
    ~RPCContext() = default;
    RPCContext(const RPCContext&) = delete;
    RPCContext& operator=(const RPCContext&) = delete;
    RPCContext(RPCContext&&) = delete;
    RPCContext& operator=(RPCContext&&) = delete;

    void SetSuccessCB(RPCSuccessCB cb);
    void SetTimeoutCB(RPCTimeoutCB cb, Duration d);
private:
    bool HasSuccessCB() const;
    bool HasTimeoutCB() const;
    const Duration& GetTimeoutDuration() const;
    void CB_Timeout();
    void CB_Success(ReceivedMessagePtr msg);

private:
    RPCSuccessCB m_success_cb;
    RPCTimeoutCB m_timeout_cb;
    Duration     m_timeout_duration;
};

class RPCManager
{
    struct ContextSlot
    {
        TimerHdl        m_timer_hdl{};
        RPCContextUPtr  m_context{};
    };

public:
    RPCManager(UniformNetwork& network);
    ~RPCManager();
    RPCManager(const RPCManager&) = delete;
    RPCManager& operator=(const RPCManager&) = delete;
    RPCManager(RPCManager&&) = delete;
    RPCManager& operator=(RPCManager&&) = delete;

    bool Init(std::unique_ptr<ConsumerConf> c_conf, std::unique_ptr<ProducerConf> p_conf);
    void Tick(DiffTime diff);
    RPCKey AsyncRPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr context);
    void RPCResponse(const MessageContext& msg_context, MsgID msg_id, MsgType msg);

    void CB_ReceviedMsg(const void* p, size_t p_len, const void* key, size_t key_len);
    CallbackManager& getCallbackManager();
private:
    RPCKey NextKey();
    void CB_SuccessKey(RPCKey key);
    void CB_TimeoutKey(RPCKey key);
    void AppendRPCContext(const RPCKey& key, RPCContextUPtr context);

    void ProcessMsg();
    void RpcReceviedMsg(ReceivedMessagePtr msg);
    void NormalReceviedMsg(ReceivedMessagePtr msg);

    TimerManager& GetTimerManager();

private:
    UniformNetwork&                     m_network;
    std::unique_ptr<detail::Consumer>   m_consumer;
    std::unique_ptr<detail::Producer>   m_producer;
    std::unique_ptr<CallbackManager>    m_cb_mgr;
    uint64_t                            m_key_sequence_id;
    std::unordered_map<RPCKey, std::shared_ptr<ContextSlot>> m_contexts;

    std::mutex                          m_mtx;
    std::queue<RPCKey>                  m_success_key;
    std::queue<RPCKey>                  m_timeout_key;
    std::queue<ReceivedMessagePtr>      m_received_msgs;
};

} // knet
