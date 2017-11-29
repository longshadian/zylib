#include "knet/RPCManager.h"

#include "knet/UniformNetwork.h"
#include "knet/Message.h"
#include "knet/CallbackManager.h"
#include "knet/FakeLog.h"

#include "knet/detail/kafka/Producer.h"
#include "knet/detail/kafka/Callback.h"

namespace knet {

RPCManager::RPCManager(detail::Producer& producer)
    : m_producer(producer)
    , m_cb_mgr(std::make_unique<CallbackManager>())
    , m_current_key()
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

RPCKey RPCManager::AsyncRPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr context)
{
    // TODO 增加计时器
    auto key = NextKey();
    auto send_msg = std::make_shared<SendMessage>(sid, msg_id, std::move(msg), key);
    m_producer.SendTo(send_msg);
    m_contexts.insert(std::make_pair(key, std::move(context)));
    return key;
}

void RPCManager::OnReceivedMsg(ReceivedMessagePtr msg)
{
    if (msg->HasRPCKey()) {
        RpcReceviedMsg(msg);
    } else {
        NormalReceviedMsg(msg);
    }
}

RPCKey RPCManager::NextKey()
{
    ++m_current_key;
    if (m_current_key == 0)
        return ++m_current_key;
    return m_current_key;
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

void RPCManager::RpcReceviedMsg(ReceivedMessagePtr msg)
{
    auto it = m_contexts.find(msg->GetKey());
    if (it == m_contexts.end())
        return;
    auto& context = it->second;
    context->m_success_cb(msg);
    m_contexts.erase(msg->GetKey());
}

void RPCManager::NormalReceviedMsg(ReceivedMessagePtr msg)
{
    auto ret = m_cb_mgr->CallbackMsg(nullptr, msg);
    if (!ret) {
        FAKE_LOG(WARNING) << "can't find msg. " << msg->GetMsgID();
    }
}

} // knet
