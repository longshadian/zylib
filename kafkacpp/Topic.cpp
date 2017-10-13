#include "Kafkacpp.h"

#include <memory>
#include <string>
#include <chrono>

#include <rdkafka.h>

namespace kafkacpp {

Topic::Topic(rd_kafka_topic_t* t)
    : m_rd_kafka_topic(t)
{
}

Topic::~Topic()
{
    if (m_rd_kafka_topic)
        ::rd_kafka_topic_destroy(m_rd_kafka_topic);
}

rd_kafka_topic_t* Topic::rd_kafka_topic()
{
    return m_rd_kafka_topic;
};

} //kafkacpp
