#pragma once

#include <rdkafkacpp.h>

namespace knet {

namespace detail {

class EventCB : public ::RdKafka::EventCb 
{
public:
    EventCB() = default;
    virtual ~EventCB() = default;
    EventCB(const EventCB&) = delete;
    EventCB& operator=(const EventCB&) = delete;
    EventCB(EventCB&&) = delete;
    EventCB& operator=(EventCB&&) = delete;

    virtual void event_cb(::RdKafka::Event&) override;
};

class DeliveryReportCB : public ::RdKafka::DeliveryReportCb 
{
public:
    DeliveryReportCB() = default;
    virtual ~DeliveryReportCB() = default;
    DeliveryReportCB(const DeliveryReportCB&) = default;
    DeliveryReportCB& operator=(const DeliveryReportCB&) = default;
    DeliveryReportCB(DeliveryReportCB&&) = default;
    DeliveryReportCB& operator=(DeliveryReportCB&&) = default;

    virtual void dr_cb(::RdKafka::Message&) override;
};

class RebalanceCB : public ::RdKafka::RebalanceCb
{
public:
    RebalanceCB() = default;
    virtual ~RebalanceCB() = default;
    RebalanceCB(const RebalanceCB&) = delete;
    RebalanceCB& operator=(const RebalanceCB&) = delete;
    RebalanceCB(RebalanceCB&&) = delete;
    RebalanceCB& operator=(RebalanceCB&&) = delete;

    virtual void rebalance_cb(::RdKafka::KafkaConsumer*
        , ::RdKafka::ErrorCode
        , std::vector<::RdKafka::TopicPartition*>&);
};

class ConsumeCB : public ::RdKafka::ConsumeCb
{
public:
    ConsumeCB() = default;
    virtual ~ConsumeCB() = default;
    ConsumeCB(const ConsumeCB&) = delete;
    ConsumeCB& operator=(const ConsumeCB&) = delete;
    ConsumeCB(ConsumeCB&&) = delete;
    ConsumeCB& operator=(ConsumeCB&&) = delete;

    virtual void consume_cb(::RdKafka::Message&, void*) override;
};

class OffsetCommitCB : public ::RdKafka::OffsetCommitCb
{
public:
    OffsetCommitCB() = default;
    virtual ~OffsetCommitCB() = default;
    OffsetCommitCB(const OffsetCommitCB&) = delete;
    OffsetCommitCB& operator=(const OffsetCommitCB&) = delete;
    OffsetCommitCB(OffsetCommitCB&&) = delete;
    OffsetCommitCB& operator=(OffsetCommitCB&&) = delete;

    virtual void offset_commit_cb(::RdKafka::ErrorCode
        , std::vector<::RdKafka::TopicPartition*>&) override;
};

class ReceiveMessageCB
{
public:
    ReceiveMessageCB() = default;
    virtual ~ReceiveMessageCB() = default;

    ReceiveMessageCB(const ReceiveMessageCB&) = delete;
    ReceiveMessageCB& operator=(const ReceiveMessageCB&) = delete;
    ReceiveMessageCB(ReceiveMessageCB&&) = delete;
    ReceiveMessageCB& operator=(ReceiveMessageCB&&) = delete;

    virtual void onError(int32_t err_no, const std::string& err_str);
    virtual void onReceived(const void* p, size_t p_len, const void* key, size_t key_len);
};

} // detail
} // knet
