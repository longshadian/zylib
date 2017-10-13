#include "Kafkacpp.h"

#include <memory>
#include <iostream>

#include <rdkafka.h>

namespace kafkacpp {

Consumer::Consumer(rd_kafka_t* t)
    : m_rd_kafka(t)
    , m_rd_kafka_topic_partition_list()
    , m_cb()
    , m_recevied_msg_cb()
    , m_recevied_error_cb()
    , m_topics()
{
}

Consumer::~Consumer()
{
    if (m_rd_kafka)
        ::rd_kafka_destroy(m_rd_kafka);
};

void Consumer::setConsumerCB(Consumer_CB cb)
{
    m_cb = std::move(cb);
}

void Consumer::setReceivedMsgCB(ReceivedMsg_CB cb)
{
    m_recevied_msg_cb = std::move(cb);
}

void Consumer::setReceivedErrorCB(ReceivedError_CB cb)
{
    m_recevied_error_cb = std::move(cb);
}

void Consumer::addTopicAndPartition(const std::string& topic, int partition)
{
    m_topics[topic] = partition;
}

bool Consumer::startConsume()
{
    auto ret = ::rd_kafka_poll_set_consumer(m_rd_kafka);
    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cout << " ret: " << ret << " " << rd_kafka_err2str(ret) << "\n";
        return false;
    }

    releaseTopicList();
    m_rd_kafka_topic_partition_list = ::rd_kafka_topic_partition_list_new(m_topics.size());
    for (const auto& it : m_topics) {
        ::rd_kafka_topic_partition_list_add(m_rd_kafka_topic_partition_list, it.first.c_str(), it.second);
    }

    ret = ::rd_kafka_assign(m_rd_kafka, m_rd_kafka_topic_partition_list);
    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cout << " ret: " << ret << " " << rd_kafka_err2str(ret) << "\n";
        return false;
    }
    return true;
}

void Consumer::poll(int32_t ms)
{
    rd_kafka_message_t* rkmessage = nullptr;
    rkmessage = ::rd_kafka_consumer_poll(m_rd_kafka, ms);
    if (rkmessage) {
        msgConsume(rkmessage);
        ::rd_kafka_message_destroy(rkmessage);
    }
}

void Consumer::cbConsume(rd_kafka_message_t * rkmessage, void *opaque)
{
    std::cout << "cb ptr: " << rkmessage->_private << "\n";
    /*
    if (m_cb) {
        m_cb(rkmessage, opaque);
    }
    */
}

void Consumer::releaseTopicList()
{
    if (m_rd_kafka_topic_partition_list)
        ::rd_kafka_topic_partition_list_destroy(m_rd_kafka_topic_partition_list);
    m_rd_kafka_topic_partition_list = nullptr;
}

void Consumer::msgConsume(rd_kafka_message_t* rkmessage) 
{
    if (rkmessage->err) {
        if (rkmessage->rkt) {
            std::cout << "topic: " << ::rd_kafka_topic_name(rkmessage->rkt)
                << " partition: " << rkmessage->partition
                << " offset: " << rkmessage->offset
                << " err_str: " << ::rd_kafka_err2str(rkmessage->err)
                << " " << ::rd_kafka_message_errstr(rkmessage)
                << "\n";
        } else {
            std::cout << "topic: unknown "
                << " partition: " << rkmessage->partition
                << " offset: " << rkmessage->offset
                << " err_str: " << ::rd_kafka_err2str(rkmessage->err)
                << " " << ::rd_kafka_message_errstr(rkmessage)
                << "\n";
        }
        if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
            rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
        return;
    }

    char* pos = reinterpret_cast<char*>(rkmessage->payload);
    std::string payload{pos, pos + rkmessage->len};

    pos = reinterpret_cast<char*>(rkmessage->key);
    std::string key{ pos, pos + rkmessage->key_len };

    std::cout << "msg:  "
        << " topic: " << ::rd_kafka_topic_name(rkmessage->rkt)
        << " partition: " << rkmessage->partition
        << " offset: " << rkmessage->offset
        << " msg: " << payload << " len: " << rkmessage->len
        << " key: " << key << " key_len: " << rkmessage->key_len;
}

} //kafkacpp
