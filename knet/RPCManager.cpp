#include "knet/RPCManager.h"

#include "knet/UniformNetwork.h"
#include "knet/kafka/Producer.h"
#include "knet/Message.h"

namespace knet {

RPCManager:: RPCManager(UniformNetwork& uniform_network)
    : m_uniform_network(uniform_network)
    , m_key()
    , m_contexts()
    , m_mtx()
    , m_success_key()
    , m_timeout_key()
{
}

RPCManager::~RPCManager()
{
}

void RPCManager::Tick(DiffTime diff)
{
    (void)diff;
}

RPCKey RPCManager::AsyncRPC(const ServiceID& sid, std::string str, RPCContextUPtr context)
{
    auto key = NextKey();
    m_uniform_network.GetProducer().SendToMessage(sid, std::move(str));
    m_contexts.insert(std::make_pair(key, std::move(context)));
    return key;
}

void RPCManager::OnReceivedMsg(MessagePtr msg)
{
    auto it = m_contexts.find(msg->m_key);
    if (it == m_contexts.end())
        return;
    auto& context = it->second;
    context->m_success_cb(msg);
    m_contexts.erase(msg->m_key);
}

RPCKey RPCManager::NextKey()
{
    ++m_key;
    if (m_key == 0)
        return ++m_key;
    return m_key;
}

void RPCManager::CB_SuccessKey(RPCKey key)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_success_key.push(key);
}

void RPCManager::CB_TimeoutKey(RPCKey key)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timeout_key.push(key);
}

} // knet
