#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>
#include <array>
#include <functional>

#include <rdkafka.h>

#include "Conf.h"
#include "Topic.h"

namespace kafkacpp {

class Producer;
using ProducerPtr = std::shared_ptr<Producer>;

class Producer
{
public:
    using DR_MSG_CB = std::function<void(rd_kafka_t*, const rd_kafka_message_t*)>;

    struct ProduceDelegate
    {
        DR_MSG_CB m_cb;
    };

public:
    Producer(ConfUPtr conf);
    ~Producer();
    Producer(const Producer& rhs) = delete;
    Producer& operator=(const Producer& rhs) = delete;
    Producer(Producer&& rhs) = delete;
    Producer& operator=(Producer&& rhs) = delete;

    rd_kafka_conf_t* rd_kafka_conf();
    rd_kafka_t* rd_kafka();
    TopicPtr createTopic(const std::string& str);

    bool init(std::string* err_str = nullptr);
    bool produce(Topic& topic, const void* payload, size_t len , const void* key, size_t key_len , DR_MSG_CB cb = DR_MSG_CB{});
    bool produce(Topic& topic, const void* payload, size_t len, DR_MSG_CB cb = DR_MSG_CB{});

    int32_t poll(int32_t ms);

    static void cbDrMsg(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage, void* opaque);
private:
    bool produceInternal(Topic& topic, const void* payload, size_t len
        , const void* key, size_t key_len, DR_MSG_CB cb);
    void releaseConf();
private:
    ConfUPtr        m_conf;
    rd_kafka_t*     m_rd_kafka;
    std::unordered_map<std::string, TopicPtr> m_topic_collection;
};

} //kafkacpp
