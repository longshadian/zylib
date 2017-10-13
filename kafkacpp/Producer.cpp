#include "Kafkacpp.h"

#include <array>
#include <iostream>

//#include "rdkafka_msg.h"
//#include "rd.h"

namespace kafkacpp {

Producer::Producer(rd_kafka_t* t)
    : m_rd_kafka(t)
    , m_topic_collection()
{

}

Producer::~Producer()
{
    if (m_rd_kafka)
        ::rd_kafka_destroy(m_rd_kafka);
}

rd_kafka_t* Producer::rd_kafka()
{
    return m_rd_kafka;
}

TopicPtr Producer::createTopic(const std::string& str)
{
    auto* rkt = ::rd_kafka_topic_new(m_rd_kafka, str.c_str(), NULL);
    if (!rkt) {
        return nullptr;
    }
    auto topic = std::make_shared<Topic>(rkt);
    m_topic_collection[str] = topic;
    return topic;
}

bool Producer::produce(Topic& topic, const void* payload, size_t len
    , const void* key, size_t key_len
    , DR_MSG_CB dr_msg_cb)
{
    return produceInternal(topic, payload, len, key, key_len, std::move(dr_msg_cb));
}

bool Producer::produce(Topic& topic, const void* payload, size_t len, DR_MSG_CB cb)
{
    return produceInternal(topic, payload, len, nullptr, 0, std::move(cb));
}

int32_t Producer::poll(int32_t ms)
{
    return ::rd_kafka_poll(m_rd_kafka, ms);
}

void Producer::cbDrMsg(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage, void* opaque)
{
    std::cout << "cb ptr: " << rkmessage->_private << "\n";
    if (!rkmessage->_private) {
        std::cout << "opaque null\n";
        return;
    }

    auto* pd = reinterpret_cast<ProduceDelegate*>(rkmessage->_private);
    pd->m_cb(rk, rkmessage);
    delete pd;
}

bool Producer::produceInternal(Topic& topic
    , const void* payload, size_t len
    , const void* key, size_t key_len
    , DR_MSG_CB dr_msg_cb)
{
    ProduceDelegate* pd = nullptr;
    if (dr_msg_cb) {
        pd = new ProduceDelegate();
        pd->m_cb = std::move(dr_msg_cb);
    }

    std::cout << "send ptr: " << pd << "\n";

    auto succ = ::rd_kafka_produce(
        topic.rd_kafka_topic(), RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY,
        const_cast<void*>(payload), len,
        key, key_len,
        pd) == 0;
    return succ;
}

}

