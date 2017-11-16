#include "knet/kafka/Consumer.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <rdkafkacpp.h>

namespace knet {

KafkaConsumer::KafkaConsumer()
    : m_server_conf(std::make_unique<KafkaConf>())
    , m_thread()
    , m_run()
    , m_consumer()
{
}

KafkaConsumer::~KafkaConsumer()
{
    stop();
    waitThreadExit();
}

bool KafkaConsumer::init(std::unique_ptr<KafkaConf> server_conf, std::unique_ptr<ConsumerCB> cb)
{
    m_server_conf = std::move(server_conf);
    if (server_cb) {
        if (server_cb->m_event_cb)
            m_server_cb->m_event_cb = std::move(server_cb->m_event_cb);
        if (server_cb->m_consume_cb)
            m_server_cb->m_consume_cb = std::move(server_cb->m_consume_cb);
        if (server_cb->m_rebalance_cb)
            m_server_cb->m_rebalance_cb = std::move(server_cb->m_rebalance_cb);
        if (server_cb->m_offset_commit_cb)
            m_server_cb->m_offset_commit_cb = std::move(server_cb->m_offset_commit_cb);
    }

    std::unique_ptr<::RdKafka::Conf> conf{ ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL) };
    std::unique_ptr<::RdKafka::Conf> tconf{ ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC) };

    std::string err_str{};
    ::RdKafka::Conf::ConfResult conf_ret = ::RdKafka::Conf::CONF_OK;
    conf_ret = conf->set("group.id", m_server_conf->m_group_id, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }
    conf_ret = conf->set("metadata.broker.list", m_server_conf->m_broker_list, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("rebalance_cb", &*m_server_cb->m_rebalance_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("consume_cb", &*m_server_cb->m_consume_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("offset_commit_cb", &*m_server_cb->m_offset_commit_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("event_cb", &*m_server_cb->m_event_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("default_topic_conf", &*tconf, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    std::unique_ptr<::RdKafka::KafkaConsumer> consumer{::RdKafka::KafkaConsumer::create(&*conf, err_str)};
    if (!consumer) {
        return false;
    }
    std::unique_ptr<::RdKafka::TopicPartition> topic{::RdKafka::TopicPartition::create(m_server_conf->m_topic_name
        , m_server_conf->m_partition) };
    auto ec = consumer->assign({ &*topic });
    if (ec) {
        return false;
    }

    m_consumer = std::move(consumer);

    m_run = true;
    std::thread t{ std::bind(&ReplayConsumer::threadRun, this) };
    m_thread = std::move(t);
    return true;
}

void KafkaConsumer::stop()
{
    m_run = false;
}

void KafkaConsumer::waitThreadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

void KafkaConsumer::flush()
{
    m_consumer->close();
}

void KafkaConsumer::threadRun()
{
    while (m_run) {
        ::RdKafka::Message* msg = m_consumer->consume(1000);
        processMsg(*msg);
        m_consumer->commitSync(msg);
        delete msg;
    }
}

void KafkaConsumer::processMsg(const ::RdKafka::Message& msg)
{
    switch (msg.err()) {
    case ::RdKafka::ERR__TIMED_OUT:
        break;
    case ::RdKafka::ERR_NO_ERROR:
        processMsgInternal(msg);
        break;
    case ::RdKafka::ERR__PARTITION_EOF:
        break;
    case ::RdKafka::ERR__UNKNOWN_TOPIC:
    case ::RdKafka::ERR__UNKNOWN_PARTITION:
        break;
    default:
        /* Errors */
        break;
    }
}

void KafkaConsumer::processMsgInternal(const ::RdKafka::Message& msg)
{
}

} // knet
