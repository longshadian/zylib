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

}

class UniformNetwork;
class CallbackManager;

using RPCSuccessCB = std::function<void(ReceivedMessagePtr msg)>;
using RPCTimeoutCB = std::function<void()>;

struct RPCContext
{
    RPCSuccessCB m_success_cb{};
    RPCTimeoutCB m_timeout_cb{};
};

using RPCContextUPtr = std::unique_ptr<RPCContext>;

class RPCManager
{
public:
    RPCManager(detail::Producer& producer);
    ~RPCManager();
    RPCManager(const RPCManager&) = delete;
    RPCManager& operator=(const RPCManager&) = delete;
    RPCManager(RPCManager&&) = delete;
    RPCManager& operator=(RPCManager&&) = delete;

    void Tick(DiffTime diff);
    RPCKey AsyncRPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr context);
    void OnReceivedMsg(ReceivedMessagePtr msg);

private:
    RPCKey NextKey();
    void CB_SuccessKey(RPCKey key);
    void CB_TimeoutKey(RPCKey key);

    void RpcReceviedMsg(ReceivedMessagePtr msg);
    void NormalReceviedMsg(ReceivedMessagePtr msg);

private:
    detail::Producer&                m_producer;
    std::unique_ptr<CallbackManager> m_cb_mgr;
    RPCKey                           m_current_key;
    std::unordered_map<RPCKey, RPCContextUPtr> m_contexts; 

    std::mutex      m_mtx;
    std::queue<RPCKey> m_success_key;
    std::queue<RPCKey> m_timeout_key;
};

} // knet
