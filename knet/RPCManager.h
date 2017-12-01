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

using RPCSuccessCB = std::function<void(ReceivedMessagePtr msg)>;
using RPCTimeoutCB = std::function<void()>;

class RPCContext
{
public:
    RPCContext() = default;
    ~RPCContext() = default;
    RPCContext(const RPCContext&) = delete;
    RPCContext& operator=(const RPCContext&) = delete;
    RPCContext(RPCContext&&) = delete;
    RPCContext& operator=(RPCContext&&) = delete;

    RPCSuccessCB m_success_cb;
    RPCTimeoutCB m_timeout_cb;
};

class RPCManager
{
public:
    RPCManager();
    ~RPCManager();
    RPCManager(const RPCManager&) = delete;
    RPCManager& operator=(const RPCManager&) = delete;
    RPCManager(RPCManager&&) = delete;
    RPCManager& operator=(RPCManager&&) = delete;

    bool Init(std::unique_ptr<ConsumerConf> c_conf, std::unique_ptr<ProducerConf> p_conf);
    void Tick(DiffTime diff);
    RPCKey AsyncRPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr context);

    void CB_ReceviedMsg(const void* p, size_t p_len, const void* key, size_t key_len);
private:
    RPCKey NextKey();
    void CB_SuccessKey(RPCKey key);
    void CB_TimeoutKey(RPCKey key);

    void ProcessMsg();
    void RpcReceviedMsg(ReceivedMessagePtr msg);
    void NormalReceviedMsg(ReceivedMessagePtr msg);

private:
    std::unique_ptr<detail::Consumer>   m_consumer;
    std::unique_ptr<detail::Producer>   m_producer;
    std::unique_ptr<CallbackManager>    m_cb_mgr;
    RPCKey                              m_current_key;
    std::unordered_map<RPCKey, RPCContextUPtr> m_contexts; 

    std::mutex                          m_mtx;
    std::queue<RPCKey>                  m_success_key;
    std::queue<RPCKey>                  m_timeout_key;
    std::queue<ReceivedMessagePtr>      m_received_msgs;
};

} // knet
