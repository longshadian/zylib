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

class KMessage
{
public:
    KMessage();
    virtual ~KMessage();

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

private:
    void ProcessMsg();

private:
    std::mutex                       m_mtx;
    std::queue<MessagePtr>           m_queue;
    std::unique_ptr<CallbackManager> m_cb_mgr;
    std::unique_ptr<Consumer>        m_consumer;
    std::unique_ptr<Producer>        m_producer;
};

} // knet
