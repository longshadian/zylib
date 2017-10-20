#include "Consumer.h"

#include <memory>
#include <iostream>

#include <rdkafka.h>

#include "Kafkacpp.h"

namespace kafkacpp {

Consumer::Consumer(ConfUPtr conf)
    : m_conf(std::move(conf))
    , m_rd_kafka()
    , m_rd_kafka_tp_list()
    , m_consumer_cb()
    , m_recevied_msg_cb()
    , m_topics()
{
}

Consumer::~Consumer()
{
    if (m_rd_kafka)
        ::rd_kafka_destroy(m_rd_kafka);
    releaseTopicList();
};

void Consumer::setConsumerCB(Consumer_CB cb)
{
    m_consumer_cb = std::move(cb);
}

void Consumer::setReceivedMsgCB(ReceivedMsg_CB cb)
{
    m_recevied_msg_cb = std::move(cb);
}

void Consumer::setCommiteCB(Commite_CB cb)
{
    m_commite_cb = std::move(cb);
}

rd_kafka_conf_t* Consumer::rd_kafka_conf()
{
    return m_conf ? m_conf->rd_kafka_conf() : nullptr;
}

bool Consumer::init(std::string* err_str)
{
    ErrStr es{};
    ::rd_kafka_conf_set_consume_cb(rd_kafka_conf(), &Consumer::cbConsume);
    ::rd_kafka_conf_set_offset_commit_cb(rd_kafka_conf(), &Consumer::cbCommite);
    ::rd_kafka_conf_set_opaque(rd_kafka_conf(), this);

    m_rd_kafka = ::rd_kafka_new(RD_KAFKA_CONSUMER, rd_kafka_conf(), es.getPtr(), es.getLen());

    auto* rkt = ::rd_kafka_topic_new(m_rd_kafka, "tp.test", nullptr);
    auto ret = ::rd_kafka_seek(rkt, 0, RD_KAFKA_OFFSET_END, 1000);
    ::rd_kafka_topic_destroy(rkt);
    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cout << "seek error: " << ret << " " << ::rd_kafka_err2str(ret) << "\n";
    }
    //return ret == RD_KAFKA_RESP_ERR_NO_ERROR;


    releaseConf();
    if (err_str)
        *err_str = es.getPtr();
    if (m_rd_kafka) {
        return true;
    }
    return false;
}

void Consumer::addTopicAndPartition(const std::string& topic, int partition)
{
    m_topics[topic] = partition;
}

int Consumer::addBrokerList(const char* brokerlist)
{
    return ::rd_kafka_brokers_add(m_rd_kafka, brokerlist);
}

bool Consumer::seekOffset(const std::string& topic, int partition, int64_t offset)
{
    auto* rkt = ::rd_kafka_topic_new(m_rd_kafka, topic.c_str(), NULL);
    auto ret = ::rd_kafka_seek(rkt, partition, offset, 1000);
    ::rd_kafka_topic_destroy(rkt);

    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cout << "seek error: " << ret << " " << ::rd_kafka_err2str(ret) << "\n";
    }
    return ret == RD_KAFKA_RESP_ERR_NO_ERROR;
}

bool Consumer::getOffset(const std::string& topic, int partition, int64_t* offset)
{
    rd_kafka_topic_partition_list_t* new_list = nullptr;
    ::rd_kafka_assignment(m_rd_kafka, &new_list);
    auto ret = ::rd_kafka_position(m_rd_kafka, new_list);

    std::cout << "rd_kafka_position ret: " << ret << "\n";

    // 分区中没有消息
    if (ret == RD_KAFKA_OFFSET_INVALID) {
        *offset = -1;
        return true;
    }
    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cout << ::rd_kafka_err2str(::rd_kafka_last_error()) << "\n";
        return false;
    }

    bool succ = false;
    auto* rktpar = ::rd_kafka_topic_partition_list_find(new_list, topic.c_str(), partition);
    if (rktpar) {
        succ = true;
        std::cout << "topic: " << rktpar->topic
            << " parition: " << rktpar->partition
            << " offset: " << rktpar->offset
            << "\n";
        *offset = rktpar->offset;
    }
    ::rd_kafka_topic_partition_list_destroy(new_list);
    return succ;
}

bool Consumer::startConsume()
{
    auto ret = ::rd_kafka_poll_set_consumer(m_rd_kafka);
    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        //std::cout << " ret: " << ret << " " << rd_kafka_err2str(ret) << "\n";
        return false;
    }

    releaseTopicList();
    m_rd_kafka_tp_list = ::rd_kafka_topic_partition_list_new(m_topics.size());
    for (const auto& it : m_topics) {
        ::rd_kafka_topic_partition_list_add(m_rd_kafka_tp_list, it.first.c_str(), it.second);
    }
    ret = ::rd_kafka_assign(m_rd_kafka, m_rd_kafka_tp_list);
    if (ret != RD_KAFKA_RESP_ERR_NO_ERROR) {
        //std::cout << " ret: " << ret << " " << rd_kafka_err2str(ret) << "\n";
        return false;
    }
    return true;
}

void Consumer::poll(int32_t ms)
{
    rd_kafka_message_t* rkmessage = nullptr;
    rkmessage = ::rd_kafka_consumer_poll(m_rd_kafka, ms);
    if (rkmessage) {
        if (m_recevied_msg_cb)
            m_recevied_msg_cb(rkmessage);
        ::rd_kafka_message_destroy(rkmessage);
    }
}

void Consumer::cbConsume(rd_kafka_message_t* rkmessage, void *opaque)
{
    auto* p_this = reinterpret_cast<Consumer*>(opaque);
    if (p_this) {
        if (p_this->m_consumer_cb)
            p_this->m_consumer_cb(rkmessage);
    }
}

void Consumer::cbCommite(rd_kafka_t* rk, rd_kafka_resp_err_t err
        , rd_kafka_topic_partition_list_t* offsets, void *opaque)
{
    auto* p_this = reinterpret_cast<Consumer*>(opaque);
    if (p_this) {
        if (p_this->m_commite_cb)
            p_this->m_commite_cb(rk, err, offsets);
    }
}

void Consumer::releaseTopicList()
{
    if (m_rd_kafka_tp_list)
        ::rd_kafka_topic_partition_list_destroy(m_rd_kafka_tp_list);
    m_rd_kafka_tp_list = nullptr;
}

void Consumer::releaseConf()
{
    if (m_conf) {
        m_conf->reset();
    }
    m_conf = nullptr;
}

} //kafkacpp
