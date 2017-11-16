#pragma once

#include <thread>
#include <atomic>

#include <rdkafkacpp.h>

#include "knet/kafka/KafkaTypes.h"

namespace knet {

class ServerConf;
class ServerCallback;
class ConsumerReceiveMessageCB;

struct KafkaConf
{
    std::string m_group_id{};
    std::string m_broker_list{};
    std::string m_topic{};
    int32_t     m_partition{};
};

struct ConsumerCB
{
    ConsumerConsumeCBUPtr       m_rebalance_cb{};
    ConsumerConsumeCBUPtr       m_consume_cb{};
    ConsumerOffsetCommitCBUPtr  m_offset_commit_cb{};
    ReplayEventCBUPtr           m_event_cb{};
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

    bool init(std::unique_ptr<KafkaConf> server_conf, std::unique_ptr<ConsumerCB> cb);
    void stop();
    void waitThreadExit();
    void flush();

private:
    void threadRun();
    void processMsg(const ::RdKafka::Message& msg);

private:
    std::thread                                 m_thread;
    std::atomic<bool>                           m_run;
    std::unique_ptr<KafkaConf>                  m_server_conf;
    std::unique_ptr<::RdKafka::KafkaConsumer>   m_consumer;
    std::unique_ptr<ConsumerCB>                 m_cb_list;
    std::unique_ptr<ConsumerReceiveMessageCB>   m_received_cb;
};

} // knet
