#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <queue>
#include <mutex>

#include "knet/KNetTypes.h"

namespace knet {

class UniformNetwork;

using RPCSuccessCB = std::function<void(MessagePtr msg)>;
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
    RPCManager(UniformNetwork& uniform_network);
    ~RPCManager();
    RPCManager(const RPCManager&) = delete;
    RPCManager& operator=(const RPCManager&) = delete;
    RPCManager(RPCManager&&) = delete;
    RPCManager& operator=(RPCManager&&) = delete;

    void Tick(DiffTime diff);
    RPCKey AsyncRPC(const ServiceID& sid, std::string str, RPCContextUPtr context);
    void OnReceivedMsg(MessagePtr msg);

private:
    RPCKey NextKey();
    void CB_SuccessKey(RPCKey key);
    void CB_TimeoutKey(RPCKey key);

private:
    UniformNetwork& m_uniform_network;
    RPCKey          m_key;
    std::unordered_map<RPCKey, RPCContextUPtr> m_contexts; 

    std::mutex      m_mtx;
    std::queue<RPCKey> m_success_key;
    std::queue<RPCKey> m_timeout_key;
};

} // knet
