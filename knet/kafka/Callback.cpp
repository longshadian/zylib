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

} // knet
