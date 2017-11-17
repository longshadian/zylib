#pragma once

#include <memory>
#include <mutex>
#include <queue>

#include "knet/KNetTypes.h"

namespace knet {

class KafkaConsumer;
class CallbackManager;
class Consumer;
class Producer;
class RPCManager;
class TimerManager;

class KMessage
{
public:
    KMessage();
    virtual ~KMessage();

    bool HasRPCKey() const;

    int32_t              m_msg_id;
    std::vector<uint8_t> m_payload;
    RPCKey               m_key;
};

class KMessageContext
{
public:
    KMessageContext();
    virtual ~KMessageContext();

};

class UniformNetwork
{
public:
    UniformNetwork();
    ~UniformNetwork();

    UniformNetwork(const UniformNetwork&) = delete;
    UniformNetwork& operator=(const UniformNetwork&) = delete;
    UniformNetwork(UniformNetwork&&) = delete;
    UniformNetwork& operator=(UniformNetwork&&) = delete;

    bool Init();
    void Tick(DiffTime diff);

    Producer& GetProducer();
    Consumer& GetConsumer();
    TimerManager& GetTimerManager();

private:
    void ProcessMsg();
    void DispatchMsg(MessagePtr msg);

private:
    std::mutex                       m_mtx;
    std::queue<MessagePtr>           m_queue;
    std::unique_ptr<CallbackManager> m_cb_mgr;
    std::unique_ptr<Consumer>        m_consumer;
    std::unique_ptr<Producer>        m_producer;
    std::unique_ptr<RPCManager>      m_rpc_manager;
    std::unique_ptr<TimerManager>    m_timer_manager;
};

} // knet
