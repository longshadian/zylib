#include "knet/UniformNetwork.h"

#include <cstring>

#include "knet/RPCManager.h"
#include "knet/TimerManager.h"
#include "knet/EventManager.h"
#include "knet/Message.h"
#include "knet/FakeLog.h"

#include "knet/detail/kafka/Consumer.h"
#include "knet/detail/kafka/Producer.h"
#include "knet/detail/kafka/Callback.h"

namespace knet {

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UniformNetwork::UniformNetwork()
{
}

UniformNetwork::~UniformNetwork()
{
}

bool UniformNetwork::Init()
{
    m_event_manager = std::make_unique<EventManager>();
    m_consumer = std::make_unique<detail::Consumer>();
    m_producer = std::make_unique<detail::Producer>();
    m_rpc_manager = std::make_unique<RPCManager>(*m_producer);
    m_timer_manager = std::make_unique<TimerManager>(*m_event_manager);
    return false;
}

void UniformNetwork::Tick(DiffTime diff)
{
    (void)diff;
    ProcessMsg();
}

void UniformNetwork::Rpc(const ServiceID& sid, MsgID msg_id, MsgType msg)
{
    // TODO
    (void)sid;
    (void)msg;
    (void)msg_id;
}

void UniformNetwork::Send(const ServiceID& sid, MsgID msg_id, MsgType msg)
{
    (void)sid;
    (void)msg_id;
    (void)msg;
}

void UniformNetwork::ProcessMsg()
{
    decltype(m_received_msgs) temp{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(temp, m_received_msgs);
    }

    while (!temp.empty()) {
        ReceivedMessagePtr msg = std::move(temp.front());
        temp.pop();
        m_rpc_manager->OnReceivedMsg(msg);
    }
}

void UniformNetwork::ReceviedMsg_CB(const void* p, size_t p_len, const void* key, size_t key_len)
{
    auto received_msg = MessageDecoder::decode(
        reinterpret_cast<const uint8_t*>(p), p_len
        , reinterpret_cast<const uint8_t*>(key), key_len);
    if (!received_msg) {
        FAKE_LOG(WARNING) << "decode msg fail\n";
        return;
    }
    std::lock_guard<std::mutex> lk(m_mtx);
    m_received_msgs.push(std::move(received_msg));
}

const ServiceID& UniformNetwork::SelfServiceID() const
{
    return m_consumer->GetServiceID();
}

} // knet
