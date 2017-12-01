#include "knet/UniformNetwork.h"

#include <cstring>

#include "knet/RPCManager.h"
#include "knet/TimerManager.h"
#include "knet/EventManager.h"
#include "knet/FakeLog.h"

namespace knet {

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UniformNetwork::UniformNetwork()
    : m_event_manager()
    , m_timer_manager()
    , m_rpc_manager()
    , m_consumer_conf()
    , m_producer_conf()
{
}

UniformNetwork::~UniformNetwork()
{
}

void UniformNetwork::setConsuerConf(std::unique_ptr<ConsumerConf> conf)
{
    m_consumer_conf = std::move(conf);
}

void UniformNetwork::setProducerConf(std::unique_ptr<ProducerConf> conf)
{
    m_producer_conf = std::move(conf);
}

void UniformNetwork::setMsgCallbackArray(Msg_Callback_Array arr)
{
    m_msg_cb_array = std::move(arr);
}

bool UniformNetwork::Init()
{
    m_event_manager = std::make_unique<EventManager>();
    if (!m_event_manager->Init())
        return false;

    m_timer_manager = std::make_unique<TimerManager>(*m_event_manager);
    if (!m_timer_manager->Init())
        return false;

    m_rpc_manager = std::make_unique<RPCManager>();
    if (!m_rpc_manager->Init(std::move(m_consumer_conf), std::move(m_producer_conf)))
        return false;
    m_rpc_manager->getCallbackManager().SetMsgCallbackArray(std::move(m_msg_cb_array));

    return true;
}

void UniformNetwork::Tick(DiffTime diff)
{
    m_event_manager->Tick(diff);
    m_timer_manager->Tick(diff);
    m_rpc_manager->Tick(diff);
}

void UniformNetwork::RPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr cb)
{
    m_rpc_manager->AsyncRPC(sid, msg_id, msg, std::move(cb));
}

void UniformNetwork::Send(const ServiceID& sid, MsgID msg_id, MsgType msg)
{
    (void)sid;
    (void)msg_id;
    (void)msg;
}

} // knet
