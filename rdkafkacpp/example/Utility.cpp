#include "Utility.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>
#include <chrono>
#include <ctime>

#include <librdkafka/rdkafkacpp.h>

namespace utility {

std::string getMsgStr(const ::RdKafka::Message& msg)
{
    if (!msg.payload())
        return {};
    const char* p = reinterpret_cast<char*>(msg.payload());
    return std::string{p, p + msg.len()};
}

std::string getKeyStr(const ::RdKafka::Message& msg)
{
    if (!msg.key_pointer())
        return {};
    const char* p = reinterpret_cast<const char*>(msg.key_pointer());
    return std::string{ p, p + msg.key_len() };
}

std::string getTmName(const ::RdKafka::Message& msg)
{
    std::string tsname = "?";
    auto ts = msg.timestamp();
    if (ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
        if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
            tsname = "create time";
        else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
            tsname = "log append time";
    }
    return tsname;
}

DeliveryReportCb::DeliveryReportCb()
{

}

DeliveryReportCb::~DeliveryReportCb()
{
}

void DeliveryReportCb::dr_cb(::RdKafka::Message &message) 
{
    if (message.err()) {
        std::cout << "producer_send_error: " << message.errstr()
            << "    topic: " << message.topic_name()
            << "    key: " << getKeyStr(message)
            << "    payload: " << getMsgStr(message)
            << "\n";
    } else {
        /*
        std::cout << "producer_send_success: " << message.topic_name()
            << " partition: " << message.partition()
            << " offset: " << message.offset()
            << " key: " << getKeyStr(message)
            << " payload: " << getMsgStr(message)
            << "\n";
            */
        /*
        if (send_dr_count == send_count) {
            auto tend = std::chrono::system_clock::now();
            auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tnow).count();
            std::cout << "cb len: " << data_size << " count: " << send_count
                << " cost: " << delta << "ms\n";
        }
        */
        /*
        if (m_cb)
            m_cb();
            */
    }
}

RebalanceCb::RebalanceCb()
{
}

RebalanceCb::~RebalanceCb()
{
}

void RebalanceCb::part_list_print(const std::vector<RdKafka::TopicPartition*>&partitions) 
{
    for (unsigned int i = 0; i < partitions.size(); i++)
        std::cerr << partitions[i]->topic() <<
        "[" << partitions[i]->partition() << "], ";
    std::cerr << "\n";
}

void RebalanceCb::rebalance_cb(RdKafka::KafkaConsumer *consumer,
        RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*> &partitions) 
{
    std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

    part_list_print(partitions);

    if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
        consumer->assign(partitions);
    } else {
        consumer->unassign();
    }
}

void msg_consume(RdKafka::Message* message, void* opaque) 
{
    switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        RdKafka::MessageTimestamp ts;
        ts = message->timestamp();
        if (false && ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
            std::string tsname = "?";
            if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
                tsname = "create time";
            else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                tsname = "log append time";
            std::cout << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
        }
        {

            //std::cout << (const char*)message->payload() << " offset " << message->offset() << "\n";
            /*
            printf("%.*s\n",
                static_cast<int>(message->len()),
                static_cast<const char *>(message->payload()));
                */
        }
        break;

    case RdKafka::ERR__PARTITION_EOF:
            std::cerr << "%% EOF reached for all partition(s)" << std::endl;
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        break;

    default:
        /* Errors */
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
    }
}

ConsumeCb::ConsumeCb()
{

}

ConsumeCb::~ConsumeCb()
{
}

void ConsumeCb::consume_cb(RdKafka::Message &msg, void *opaque) 
{
    std::cout << "consumer cb\n";
    msg_consume(&msg, opaque);
}

}
