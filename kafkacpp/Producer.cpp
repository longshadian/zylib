#include "Producer.h"

#include <array>
#include <iostream>

#include "Kafkacpp.h"

namespace kafkacpp {

Producer::Producer(ConfUPtr conf)
    : m_conf(std::move(conf))
    , m_rd_kafka()
    , m_topic_collection()
{

}

Producer::~Producer()
{
    if (m_rd_kafka)
        ::rd_kafka_destroy(m_rd_kafka);
}

bool Producer::init(std::string* err_str)
{
    ErrStr es{};
    ::rd_kafka_conf_set_dr_msg_cb(rd_kafka_conf(), &Producer::cbDrMsg);
    m_rd_kafka = ::rd_kafka_new(RD_KAFKA_PRODUCER, rd_kafka_conf(), es.getPtr(), es.getLen());
    releaseConf();
    if (err_str)
        *err_str = es.getPtr();
    if (m_rd_kafka)
        return true;
    return false;
}

rd_kafka_conf_t* Producer::rd_kafka_conf()
{
    return m_conf ? m_conf->rd_kafka_conf() : nullptr;
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
    if (!rkmessage->_private) {
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

    auto ret = ::rd_kafka_produce(
        topic.rd_kafka_topic(), 0, RD_KAFKA_MSG_F_COPY,
        const_cast<void*>(payload), len,
        key, key_len,
        pd);
    return ret == 0;
}

void Producer::releaseConf()
{
    // ÏÈreset
    if (m_conf) {
        m_conf->reset();
    }
    m_conf = nullptr;
}

}

