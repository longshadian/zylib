#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>
#include <array>
#include <functional>

#include <rdkafka.h>

namespace kafkacpp {

class Topic;
using TopicPtr = std::shared_ptr<Topic>;

class Topic
{
public:
    Topic(rd_kafka_topic_t* t);
    ~Topic();
    Topic(const Topic& rhs) = delete;
    Topic& operator=(const Topic& rhs) = delete;
    Topic(Topic&& rhs) = delete;
    Topic& operator=(Topic&& rhs) = delete;

    rd_kafka_topic_t* rd_kafka_topic();

private:
    rd_kafka_topic_t* m_rd_kafka_topic;
};

} //kafkacpp
