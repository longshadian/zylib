#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>
#include <array>
#include <functional>

#include <rdkafka.h>

#include "Conf.h"

namespace kafkacpp {
    
class Consumer;
using ConsumerPtr = std::shared_ptr<Consumer>;

class Consumer
{
public:
    using Consumer_CB = std::function<void(rd_kafka_message_t*)>;
    using ReceivedMsg_CB = std::function<void(rd_kafka_message_t*)>;
    using Commite_CB = std::function<void(rd_kafka_t*, rd_kafka_resp_err_t
        , rd_kafka_topic_partition_list_t *)>;

public:
    Consumer(ConfUPtr conf);
    ~Consumer();

    Consumer(const Consumer& rhs) = delete;
    Consumer& operator=(const Consumer& rhs) = delete;
    Consumer(Consumer&& rhs) = delete;
    Consumer& operator=(Consumer&& rhs) = delete;
    
    void setConsumerCB(Consumer_CB cb);
    void setReceivedMsgCB(ReceivedMsg_CB cb);
    void setCommiteCB(Commite_CB cb);

    rd_kafka_conf_t* rd_kafka_conf();

    bool init(std::string* err_str = nullptr);
    void addTopicAndPartition(const std::string& topic, int partition);
    int addBrokerList(const char* brokerlist);
    bool seekOffset(const std::string& topic, int partition, int64_t offset);

    // offset等于-1:分区中没有消息
    bool getOffset(const std::string& topic, int partition, int64_t* offset);
    bool startConsume();
    void poll(int32_t ms);

    static void cbConsume(rd_kafka_message_t * rkmessage, void *opaque);
    static void cbCommite(rd_kafka_t* rk, rd_kafka_resp_err_t err
        , rd_kafka_topic_partition_list_t *offsets, void *opaque);
private:
    void releaseTopicList();
    void releaseConf();

private:
    ConfUPtr m_conf;

    rd_kafka_t* m_rd_kafka;
    rd_kafka_topic_partition_list_t* m_rd_kafka_tp_list;

    Consumer_CB         m_consumer_cb;
    ReceivedMsg_CB      m_recevied_msg_cb;
    Commite_CB          m_commite_cb;

    std::unordered_map<std::string, int> m_topics;
};

} //kafkacpp
