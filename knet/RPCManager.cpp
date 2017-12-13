#include "knet/RPCManager.h"

#include "knet/UniformNetwork.h"
#include "knet/Message.h"
#include "knet/CallbackManager.h"
#include "knet/FakeLog.h"
#include "knet/TimerManager.h"

#include "knet/detail/kafka/Producer.h"
#include "knet/detail/kafka/Consumer.h"
#include "knet/detail/kafka/Callback.h"
#include "knet/detail/AsyncReceivedMsgCB.h"

namespace knet {

bool RPCContext::HasSuccessCB() const { return m_success_cb ? true : false; }
bool RPCContext::HasTimeoutCB() const { return m_timeout_cb ? true : false; }
void RPCContext::SetSuccessCB(RPCSuccessCB cb) { m_success_cb = std::move(cb); }
void RPCContext::SetTimeoutCB(RPCTimeoutCB cb, Duration d) { m_timeout_cb = std::move(cb); m_timeout_duration = d; }
const Duration& RPCContext::GetTimeoutDuration() const { return m_timeout_duration; }
void RPCContext::CB_Timeout()
{
    if (HasTimeoutCB())
        m_timeout_cb();
}

void RPCContext::CB_Success(ReceivedMessagePtr msg)
{
    if (HasSuccessCB())
        m_success_cb(msg);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
RPCManager::RPCManager(UniformNetwork& network)
    : m_network(network)
    , m_consumer()
    , m_producer()
    , m_cb_mgr(std::make_unique<CallbackManager>())
    , m_key_sequence_id()
    , m_contexts()
    , m_mtx()
    , m_success_key()
    , m_timeout_key()
    , m_received_msgs()
{
}

RPCManager::~RPCManager()
{
}

bool RPCManager::Init(std::unique_ptr<ConsumerConf> c_conf, std::unique_ptr<ProducerConf> p_conf)
{
    m_consumer = std::make_unique<detail::Consumer>();
    if (!m_consumer->Init(std::move(c_conf)
        , std::make_unique<detail::AsyncReceivedMsgCB>(*this)
        , nullptr)) {
        return false;
    }
    m_producer = std::make_unique<detail::Producer>();
    if (!m_producer->Init(std::move(p_conf), nullptr)) {
        return false;
    }
    m_cb_mgr = std::make_unique<CallbackManager>();
    return true;
}

void RPCManager::Tick(DiffTime diff)
{
    (void)diff;
    ProcessMsg();
}

RPCKey RPCManager::AsyncRPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr context)
{
    auto key = NextKey();
    auto send_msg = std::make_shared<SendMessage>(sid, msg_id, std::move(msg), key);
    m_producer->SendTo(send_msg);
    AppendRPCContext(key, std::move(context));
    return key;
}

void RPCManager::RPCResponse(const ServiceID& sid, MsgID msg_id, MsgType msg, const RPCKey& key)
{
    auto send_msg = std::make_shared<SendMessage>(sid, msg_id, std::move(msg), key);
    m_producer->SendTo(send_msg);
}

RPCKey RPCManager::NextKey()
{
    ++m_key_sequence_id;
    return m_consumer->GetServiceID() + std::to_string(m_key_sequence_id);
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

void RPCManager::AppendRPCContext(const RPCKey& key, RPCContextUPtr context)
{
    if (context) {
        auto slot = std::make_shared<ContextSlot>();
        slot->m_context = std::move(context);
        m_contexts.insert(std::make_pair(key, slot));

        // 增加定时器
        if (slot->m_context->HasTimeoutCB()) {
            auto timer_hdl = m_network.GetTimerManager().AddTimer([slot] 
                { 
                    slot->m_context->CB_Timeout(); 
                }
                , slot->m_context->GetTimeoutDuration()
            );
            slot->m_timer_hdl = timer_hdl;
        }
    }
}

void RPCManager::ProcessMsg()
{
    decltype(m_received_msgs) temp{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        std::swap(temp, m_received_msgs);
    }

    while (!temp.empty()) {
        ReceivedMessagePtr msg = std::move(temp.front());
        temp.pop();
        RpcReceviedMsg(msg);
    }
}

void RPCManager::RpcReceviedMsg(ReceivedMessagePtr msg)
{
    auto it = m_contexts.find(msg->GetKey());
    if (it == m_contexts.end()) {
        NormalReceviedMsg(msg);
        return;
    }
    auto& slot = it->second;
    slot->m_context->CB_Success(msg);
    if (slot->m_timer_hdl) {
        m_network.GetTimerManager().CancelTimer(slot->m_timer_hdl);
    }
    m_contexts.erase(it);
}

void RPCManager::NormalReceviedMsg(ReceivedMessagePtr msg)
{
    auto msg_context = std::make_shared<ReceivedMessageContext>(*this);
    auto ret = m_cb_mgr->CallbackMsg(msg_context, msg);
    if (!ret) {
        FAKE_LOG(WARNING) << "can't find msg. " << msg->GetMsgID();
    }
}

void RPCManager::CB_ReceviedMsg(const void* p, size_t p_len, const void* key, size_t key_len)
{
    auto received_msg = MessageDecoder::Decode(
        reinterpret_cast<const uint8_t*>(p), p_len
        , reinterpret_cast<const uint8_t*>(key), key_len);
    if (!received_msg) {
        FAKE_LOG(WARNING) << "decode msg fail";
        return;
    }
    std::lock_guard<std::mutex> lk(m_mtx);
    m_received_msgs.push(std::move(received_msg));
}

CallbackManager& RPCManager::getCallbackManager()
{
    return *m_cb_mgr;
}

TimerManager& RPCManager::GetTimerManager()
{
    return m_network.GetTimerManager();
}

} // knet
