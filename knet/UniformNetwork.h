#pragma once

#include "knet/KNetTypes.h"
#include "knet/CallbackManager.h"

namespace knet {

class CallbackManager;
class RPCManager;
class TimerManager;
class EventManager;

class UniformNetwork
{
public:
    UniformNetwork();
    ~UniformNetwork();
    UniformNetwork(const UniformNetwork&) = delete;
    UniformNetwork& operator=(const UniformNetwork&) = delete;
    UniformNetwork(UniformNetwork&&) = delete;
    UniformNetwork& operator=(UniformNetwork&&) = delete;

    void                SetConsuerConf(std::unique_ptr<ConsumerConf> conf);
    void                SetProducerConf(std::unique_ptr<ProducerConf> conf);
    void                SetMsgCallbackArray(Msg_Callback_Array arr);

    bool                Init();
    void                Tick(DiffTime diff);
    void                RPC(const ServiceID& sid, MsgID msg_id, MsgType msg, RPCContextUPtr cb);
    void                Send(const ServiceID& sid, MsgID msg_id, MsgType msg);

    TimerManager&       GetTimerManager();
    const TimerManager& GetTimerManager() const;
private:
    std::unique_ptr<EventManager>   m_event_manager;
    std::unique_ptr<TimerManager>   m_timer_manager;
    std::unique_ptr<RPCManager>     m_rpc_manager;

    std::unique_ptr<ConsumerConf>   m_consumer_conf;
    std::unique_ptr<ProducerConf>   m_producer_conf;
    Msg_Callback_Array              m_msg_cb_array;
};

} // knet
