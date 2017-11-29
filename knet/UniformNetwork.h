#pragma once

#include <memory>
#include <mutex>
#include <queue>

#include "knet/KNetTypes.h"
//#include "knet/kafka/Callback.h"

namespace knet {

namespace detail {
class AsyncReceivedMsgCB;
class KafkaConsumer;
class Consumer;
class Producer;

} // detail

class CallbackManager;
class RPCManager;
class TimerManager;
class EventManager;

class UniformNetwork
{
    friend class detail::AsyncReceivedMsgCB;

public:
    UniformNetwork();
    ~UniformNetwork();
    UniformNetwork(const UniformNetwork&) = delete;
    UniformNetwork& operator=(const UniformNetwork&) = delete;
    UniformNetwork(UniformNetwork&&) = delete;
    UniformNetwork& operator=(UniformNetwork&&) = delete;

    bool Init();
    void Tick(DiffTime diff);
    void Rpc(const ServiceID& sid, MsgID msg_id, MsgType msg);
    void Send(const ServiceID& sid, MsgID msg_id, MsgType msg);

private:
    void ProcessMsg();
    void ReceviedMsg_CB(const void* p, size_t p_len, const void* key, size_t key_len);

    const ServiceID& SelfServiceID() const;

private:
    std::unique_ptr<EventManager>    m_event_manager;
    std::unique_ptr<detail::Consumer> m_consumer;
    std::unique_ptr<detail::Producer> m_producer;
    std::unique_ptr<RPCManager>      m_rpc_manager;
    std::unique_ptr<TimerManager>    m_timer_manager;

    std::mutex                       m_mtx;
    std::queue<ReceivedMessagePtr>   m_received_msgs;
};

} // knet
