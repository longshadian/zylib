#include <string>
#include <functional>

#include <librdkafka/rdkafkacpp.h>

namespace utility {

std::string getMsgStr(const ::RdKafka::Message& msg);
std::string getKeyStr(const ::RdKafka::Message& msg);
std::string getTmName(const ::RdKafka::Message& msg);

class DeliveryReportCb : public ::RdKafka::DeliveryReportCb 
{
public:
    DeliveryReportCb();
    virtual ~DeliveryReportCb();

    virtual void dr_cb(::RdKafka::Message &message) override;
};


class RebalanceCb : public ::RdKafka::RebalanceCb 
{
public:
    RebalanceCb();
    virtual ~RebalanceCb();

    virtual void rebalance_cb(RdKafka::KafkaConsumer *consumer,
        RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*> &partitions) override;
private:
    static void part_list_print(const std::vector<RdKafka::TopicPartition*>&partitions);
};

void msg_consume(RdKafka::Message* message, void* opaque);

class ConsumeCb : public ::RdKafka::ConsumeCb 
{
public:
    ConsumeCb();
    virtual ~ConsumeCb();

    virtual void consume_cb(RdKafka::Message &msg, void *opaque) override;
};

}

