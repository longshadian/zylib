#pragma once

#include <rdkafkacpp.h>

namespace knet {

class ReplayEventCB : public ::RdKafka::EventCb 
{
public:
    ReplayEventCB() = default;
    virtual ~ReplayEventCB() = default;
    ReplayEventCB(const ReplayEventCB&) = delete;
    ReplayEventCB& operator=(const ReplayEventCB&) = delete;
    ReplayEventCB(ReplayEventCB&&) = delete;
    ReplayEventCB& operator=(ReplayEventCB&&) = delete;

    virtual void event_cb(::RdKafka::Event&) override;
};

class ProducerDeliveryReportCB : public ::RdKafka::DeliveryReportCb 
{
public:
    ProducerDeliveryReportCB() = default;
    virtual ~ProducerDeliveryReportCB() = default;
    ProducerDeliveryReportCB(const ProducerDeliveryReportCB&) = default;
    ProducerDeliveryReportCB& operator=(const ProducerDeliveryReportCB&) = default;
    ProducerDeliveryReportCB(ProducerDeliveryReportCB&&) = default;
    ProducerDeliveryReportCB& operator=(ProducerDeliveryReportCB&&) = default;

    virtual void dr_cb(::RdKafka::Message&) override;
};

class ConsumerRebalanceCB : public ::RdKafka::RebalanceCb
{
public:
    ConsumerRebalanceCB() = default;
    virtual ~ConsumerRebalanceCB() = default;
    ConsumerRebalanceCB(const ConsumerRebalanceCB&) = delete;
    ConsumerRebalanceCB& operator=(const ConsumerRebalanceCB&) = delete;
    ConsumerRebalanceCB(ConsumerRebalanceCB&&) = delete;
    ConsumerRebalanceCB& operator=(ConsumerRebalanceCB&&) = delete;

    virtual void rebalance_cb(::RdKafka::KafkaConsumer*
        , ::RdKafka::ErrorCode
        , std::vector<::RdKafka::TopicPartition*>&);
};

class ConsumerConsumeCB : public ::RdKafka::ConsumeCb
{
public:
    ConsumerConsumeCB() = default;
    virtual ~ConsumerConsumeCB() = default;
    ConsumerConsumeCB(const ConsumerConsumeCB&) = delete;
    ConsumerConsumeCB& operator=(const ConsumerConsumeCB&) = delete;
    ConsumerConsumeCB(ConsumerConsumeCB&&) = delete;
    ConsumerConsumeCB& operator=(ConsumerConsumeCB&&) = delete;

    virtual void consume_cb(::RdKafka::Message&, void*) override;
};

class ConsumerOffsetCommitCB : public ::RdKafka::OffsetCommitCb
{
public:
    ConsumerOffsetCommitCB() = default;
    virtual ~ConsumerOffsetCommitCB() = default;
    ConsumerOffsetCommitCB(const ConsumerOffsetCommitCB&) = delete;
    ConsumerOffsetCommitCB& operator=(const ConsumerOffsetCommitCB&) = delete;
    ConsumerOffsetCommitCB(ConsumerOffsetCommitCB&&) = delete;
    ConsumerOffsetCommitCB& operator=(ConsumerOffsetCommitCB&&) = delete;

    virtual void offset_commit_cb(::RdKafka::ErrorCode
        , std::vector<::RdKafka::TopicPartition*>&) override;
};

class ConsumerReceiveMessageCB
{
public:
    ConsumerReceiveMessageCB() = default;
    virtual ~ConsumerReceiveMessageCB() = default;

    ConsumerReceiveMessageCB(const ConsumerReceiveMessageCB&) = delete;
    ConsumerReceiveMessageCB& operator=(const ConsumerReceiveMessageCB&) = delete;
    ConsumerReceiveMessageCB(ConsumerReceiveMessageCB&&) = delete;
    ConsumerReceiveMessageCB& operator=(ConsumerReceiveMessageCB&&) = delete;

    virtual void onError(int32_t err_no, const std::string& err_str);
    virtual void onReceived(const void* p, size_t p_len, const void* key, size_t key_len);
}

} // knet
