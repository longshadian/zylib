#include "knet/detail/kafka/Callback.h"

namespace knet {

namespace detail {

void EventCB::event_cb(::RdKafka::Event&)
{
}

void DeliveryReportCB::dr_cb(::RdKafka::Message&)
{
}

void RebalanceCB::rebalance_cb(::RdKafka::KafkaConsumer*
    , ::RdKafka::ErrorCode
    , std::vector<::RdKafka::TopicPartition*>&)
{
}

void ConsumeCB::consume_cb(::RdKafka::Message&, void*)
{
}

void OffsetCommitCB::offset_commit_cb(::RdKafka::ErrorCode
    , std::vector<::RdKafka::TopicPartition*>&) 
{
}

void ReceiveMessageCB::onError(int32_t err_no, const std::string& err_str)
{
    (void)err_no; (void)err_str;
}

void ReceiveMessageCB::onReceived(const void* p, size_t p_len, const void* key, size_t key_len)
{
    (void)p; (void)p_len; (void)key; (void)key_len;
}

} // detail

} // knet
