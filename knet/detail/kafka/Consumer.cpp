#include "knet/detail/kafka/Consumer.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <rdkafkacpp.h>

#include "knet/detail/kafka/Callback.h"

namespace knet {

namespace detail {

Consumer::Consumer()
    : m_thread()
    , m_run()
    , m_server_conf(std::make_unique<KafkaConf>())
    , m_consumer()
    , m_consumer_cb(std::make_unique<ConsumerCB>())
    , m_received_cb(std::make_unique<ReceiveMessageCB>())
{
    m_consumer_cb->m_rebalance_cb = std::make_unique<RebalanceCB>();
    m_consumer_cb->m_consume_cb = std::make_unique<ConsumeCB>();
    m_consumer_cb->m_offset_commit_cb = std::make_unique<OffsetCommitCB>();
    m_consumer_cb->m_event_cb = std::make_unique<EventCB>();
}

Consumer::~Consumer()
{
    stop();
    waitThreadExit();
}

bool Consumer::init(std::unique_ptr<KafkaConf> server_conf
    , std::unique_ptr<ReceiveMessageCB> msg_cb 
    , std::unique_ptr<ConsumerCB> cb)
{
    m_server_conf = std::move(server_conf);
    if (cb) {
        if (cb->m_event_cb)
            m_consumer_cb->m_event_cb = std::move(cb->m_event_cb);
        if (cb->m_consume_cb)
            m_consumer_cb->m_consume_cb = std::move(cb->m_consume_cb);
        if (cb->m_rebalance_cb)
            m_consumer_cb->m_rebalance_cb = std::move(cb->m_rebalance_cb);
        if (cb->m_offset_commit_cb)
            m_consumer_cb->m_offset_commit_cb = std::move(cb->m_offset_commit_cb);
    }
    if (msg_cb)
        m_received_cb = std::move(msg_cb);

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

    conf_ret = conf->set("rebalance_cb", &*m_consumer_cb->m_rebalance_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("consume_cb", &*m_consumer_cb->m_consume_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("offset_commit_cb", &*m_consumer_cb->m_offset_commit_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("event_cb", &*m_consumer_cb->m_event_cb, err_str);
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
    std::unique_ptr<::RdKafka::TopicPartition> topic{::RdKafka::TopicPartition::create(m_server_conf->m_topic
        , m_server_conf->m_partition) };
    auto ec = consumer->assign({ &*topic });
    if (ec) {
        return false;
    }
    m_consumer = std::move(consumer);

    m_run = true;
    std::thread t{ std::bind(&Consumer::threadRun, this) };
    m_thread = std::move(t);
    return true;
}

void Consumer::stop()
{
    m_run = false;
}

void Consumer::waitThreadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

void Consumer::flush()
{
    m_consumer->close();
}

void Consumer::threadRun()
{
    while (m_run) {
        ::RdKafka::Message* msg = m_consumer->consume(1000);
        processMsg(*msg);
        m_consumer->commitSync(msg);
        delete msg;
    }
}

void Consumer::processMsg(const ::RdKafka::Message& msg)
{
    switch (msg.err()) {
    case ::RdKafka::ERR__TIMED_OUT:
    case ::RdKafka::ERR_NO_ERROR:
    case ::RdKafka::ERR__PARTITION_EOF:
        m_received_cb->onReceived(msg.payload(), msg.len(), msg.key_pointer(), msg.key_len());
        return;
    case ::RdKafka::ERR__UNKNOWN_TOPIC:
    case ::RdKafka::ERR__UNKNOWN_PARTITION:
        break;
    default:
        /* Errors */
        break;
    }
    m_received_cb->onError(msg.err(), msg.errstr());
}

} // detail

} // knet
