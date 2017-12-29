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

void UniformNetwork::SetConsuerConf(std::unique_ptr<ConsumerConf> conf)
{
    m_consumer_conf = std::move(conf);
}

void UniformNetwork::SetProducerConf(std::unique_ptr<ProducerConf> conf)
{
    m_producer_conf = std::move(conf);
}

void UniformNetwork::SetMsgCallbackArray(MsgCallbackArray arr)
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

    m_rpc_manager = std::make_unique<RPCManager>(*this);
    if (!m_rpc_manager->Init(std::move(m_consumer_conf), std::move(m_producer_conf)))
        return false;
    m_rpc_manager->GetCallbackManager().SetMsgCallbackArray(std::move(m_msg_cb_array));

    return true;
}

void UniformNetwork::Tick(DiffTime diff)
{
    m_event_manager->Tick(diff);
    m_timer_manager->Tick(diff);
    m_rpc_manager->Tick(diff);
}

void UniformNetwork::RPC(ServiceID sid, MsgID msg_id, MsgType msg, RPCContextUPtr cb)
{
    m_rpc_manager->AsyncRPC(std::move(sid), msg_id, msg, std::move(cb));
}

void UniformNetwork::RouteMessage(ServiceID sid, MsgID msg_id, MsgType msg)
{
    m_rpc_manager->RouteMessage(std::move(sid), std::move(msg_id), std::move(msg));
}

const ServiceID& UniformNetwork::GetServiceID() const
{
    return m_rpc_manager->GetServiceID();
}

TimerManager& UniformNetwork::GetTimerManager()
{
    return *m_timer_manager;
}

const TimerManager& UniformNetwork::GetTimerManager() const
{
    return *m_timer_manager;
}

} // knet
