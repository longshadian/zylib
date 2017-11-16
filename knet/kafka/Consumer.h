#pragma once

#include <thread>
#include <atomic>

#include <rdkafkacpp.h>

namespace knet {

class ServerConf;
class ServerCallback;
class PersistenceManager;
class ReceivedReplayCB;

struct KafkaConf
{
    std::string m_group_id{};
    std::string m_broker_list{};
};

struct ConsumerCB
{
    std::shared_ptr<ConsumerRebalanceCB>     m_rebalance_cb;
    std::shared_ptr<ConsumerConsumeCB>       m_consume_cb;
    ConsumerOffsetCommitCB  m_offset_commit_cb;
    ReplayEventCB           m_event_cb;
};

class KafkaConsumer
{
public:
    KafkaConsumer();
    ~KafkaConsumer();
    KafkaConsumer(const KafkaConsumer&) = delete;
    KafkaConsumer& operator=(const KafkaConsumer&) = delete;
    KafkaConsumer(KafkaConsumer&&) = delete;
    KafkaConsumer& operator=(KafkaConsumer&&) = delete;

    bool init(std::unique_ptr<KafkaConf> server_conf, std::unique_ptr<ConsumerCB> cb);
    void stop();
    void waitThreadExit();
    void flush();

private:
    void threadRun();
    void processMsg(const ::RdKafka::Message& msg);
    void processMsgInternal(const ::RdKafka::Message& msg);

private:
    std::unique_ptr<KafkaConf>                  m_server_conf;
    std::thread                                 m_thread;
    std::atomic<bool>                           m_run;
    std::unique_ptr<ConsumerCB>                 m_server_cb;
    std::unique_ptr<::RdKafka::KafkaConsumer>   m_consumer;
};

} // knet
