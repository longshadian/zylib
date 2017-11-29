#pragma once

#include <thread>
#include <atomic>

#include <rdkafkacpp.h>

#include "knet/KNetTypes.h"
#include "knet/detail/kafka/KafkaTypes.h"

namespace knet {

namespace detail {

class ReceiveMessageCB;

struct KafkaConf
{
    std::string m_group_id{};
    std::string m_broker_list{};
    std::string m_topic{};
    int32_t     m_partition{};
};

struct ConsumerCB
{
    RebalanceCBUPtr     m_rebalance_cb{};
    ConsumeCBUPtr       m_consume_cb{};
    OffsetCommitCBUPtr  m_offset_commit_cb{};
    EventCBUPtr         m_event_cb{};
};

class Consumer
{
public:
    Consumer();
    ~Consumer();
    Consumer(const Consumer&) = delete;
    Consumer& operator=(const Consumer&) = delete;
    Consumer(Consumer&&) = delete;
    Consumer& operator=(Consumer&&) = delete;

    bool init(std::unique_ptr<KafkaConf> server_conf
        , std::unique_ptr<ReceiveMessageCB> msg_cb 
        , std::unique_ptr<ConsumerCB> cb);
    void stop();
    void waitThreadExit();
    void flush();
    const ServiceID& GetServiceID() const;

private:
    void threadRun();
    void processMsg(const ::RdKafka::Message& msg);

private:
    std::thread                                 m_thread;
    std::atomic<bool>                           m_run;
    std::unique_ptr<KafkaConf>                  m_server_conf;
    std::unique_ptr<::RdKafka::KafkaConsumer>   m_consumer;
    std::unique_ptr<ConsumerCB>                 m_consumer_cb;
    std::unique_ptr<ReceiveMessageCB>           m_received_cb;
};

} // detail

} // knet
