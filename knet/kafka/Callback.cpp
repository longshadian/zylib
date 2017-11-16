#include "knet/kafka/Callback.h"

namespace knet {

void ReplayEventCB::event_cb(::RdKafka::Event&)
{
}

void ProducerDeliveryReportCB::dr_cb(::RdKafka::Message&)
{
}

void ConsumerRebalanceCB::rebalance_cb(::RdKafka::KafkaConsumer*
    , ::RdKafka::ErrorCode
    , std::vector<::RdKafka::TopicPartition*>&)
{
}

void ConsumerConsumeCB::consume_cb(::RdKafka::Message&, void*)
{
}

void ConsumerOffsetCommitCB::offset_commit_cb(::RdKafka::ErrorCode
    , std::vector<::RdKafka::TopicPartition*>&) 
{
}

void ConsumerReceiveMessageCB::onError(int32_t err_no, const std::string& err_str)
{
    (void)err_no; (void)err_str;
}

void ConsumerReceiveMessageCB::onReceived(const void* p, size_t p_len, const void* key, size_t key_len)
{
    (void)p; (void)p_len; (void)key; (void)key_len;
}

} // knet
