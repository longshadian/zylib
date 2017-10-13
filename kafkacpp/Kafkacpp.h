#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>
#include <array>
#include <functional>

#include <rdkafka.h>

namespace kafkacpp {

class Producer;
class Consumer;
class Conf;
class Topic;

using ProducerPtr = std::shared_ptr<Producer>;
using ConsumerPtr = std::shared_ptr<Consumer>;
using ConfPtr = std::shared_ptr<Conf>;
using TopicPtr = std::shared_ptr<Topic>;

struct ErrStr
{
    std::array<char, 512> m_data;
    char* getPtr() { return m_data.data(); }
    size_t getLen() const { return m_data.size(); }
};

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

class Producer
{
public:
    using DR_MSG_CB = std::function<void(rd_kafka_t*, const rd_kafka_message_t*)>;

    struct ProduceDelegate
    {
        DR_MSG_CB m_cb;
    };

public:
    Producer(rd_kafka_t* t);
    ~Producer();
    Producer(const Producer& rhs) = delete;
    Producer& operator=(const Producer& rhs) = delete;
    Producer(Producer&& rhs) = delete;
    Producer& operator=(Producer&& rhs) = delete;

    rd_kafka_t* rd_kafka();

    TopicPtr createTopic(const std::string& str);
    bool produce(Topic& topic, const void* payload, size_t len
        , const void* key = nullptr, size_t key_len = 0
        , DR_MSG_CB cb = DR_MSG_CB{});
    bool produce(Topic& topic, const void* payload, size_t len, DR_MSG_CB cb = DR_MSG_CB{});

    int32_t poll(int32_t ms);


    static void cbDrMsg(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage, void* opaque);
private:
    bool produceInternal(Topic& topic, const void* payload, size_t len
        , const void* key, size_t key_len, DR_MSG_CB cb);

private:
    rd_kafka_t* m_rd_kafka;
    std::unordered_map<std::string, TopicPtr> m_topic_collection;
};

class Consumer
{
public:
    using Consumer_CB = std::function<void(rd_kafka_message_t*, void*)>;

    using ReceivedMsg_CB = std::function<void(const void*, size_t)>;
    using ReceivedError_CB = std::function<void(int32_t, const char*)>;

public:
    Consumer(rd_kafka_t* t);
    ~Consumer();

    Consumer(const Consumer& rhs) = delete;
    Consumer& operator=(const Consumer& rhs) = delete;
    Consumer(Consumer&& rhs) = delete;
    Consumer& operator=(Consumer&& rhs) = delete;
    
    void setConsumerCB(Consumer_CB cb);
    void setReceivedMsgCB(ReceivedMsg_CB cb);
    void setReceivedErrorCB(ReceivedError_CB cb);

    void addTopicAndPartition(const std::string& topic, int partition);
    bool startConsume();
    void poll(int32_t ms);

    static void cbConsume(rd_kafka_message_t * rkmessage, void *opaque);
private:
    void releaseTopicList();
    void msgConsume(rd_kafka_message_t* rkmessage);

private:
    rd_kafka_t* m_rd_kafka;
    rd_kafka_topic_partition_list_t* m_rd_kafka_topic_partition_list;

    Consumer_CB         m_cb;
    ReceivedMsg_CB      m_recevied_msg_cb;
    ReceivedError_CB    m_recevied_error_cb;

    std::unordered_map<std::string, int> m_topics;
};

class Conf 
{
public:
    Conf();
    ~Conf();
    Conf(const Conf& rhs) = delete;
    Conf& operator=(const Conf& rhs) = delete;
    Conf(Conf&& rhs) = delete;
    Conf& operator=(Conf&& rhs) = delete;

    rd_kafka_conf_t* rd_kafka_conf();

    ProducerPtr createProduer(std::string* str = nullptr);
    ConsumerPtr createConsumer(std::string* str = nullptr);
    bool set(const char* name, const char* value, std::string* str = nullptr, rd_kafka_conf_res_t* val = nullptr);

private:
    rd_kafka_conf_t* m_rd_kafka_conf;
};

} //kafkacpp
