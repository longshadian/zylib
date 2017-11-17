#include "knet/RPCManager.h"

#include "knet/UniformNetwork.h"
#include "knet/kafka/Producer.h"

namespace knet {

RPCManager::RPCManager()
{
}

RPCManager::~RPCManager()
{
}

void RPCManager::Tick(DiffTime diff)
{

}

RPCKey RPCManager::AsyncRPC(const ServiceID& sid, std::string str, RPCContextUPtr context)
{
    auto key = NextKey();
    m_uniform_network.GetProducer().SendToMessage(str);
    m_contexts.insert({key, std::move(context)});
    return key;
}

RPCKey RPCManager::NextKey()
{
    return ++m_key;
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
