#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>
#include <chrono>
#include <ctime>

#include <algorithm>
#include <numeric>

#include <rdkafkacpp.h>
#include "Utility.h"

static bool consumer_run = true;
static bool producer_run = true;
static void sigterm(int sig) {
    consumer_run = false;
}

::RdKafka::Producer* producer;
::RdKafka::Topic*    topic;

void startProducer(std::thread* tout)
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    ::RdKafka::Conf *conf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL);
    conf->set("group.id", "xxx", errstr);
    conf->set("metadata.broker.list", brokers, errstr);
    utility::DeliveryReportCb* ex_dr_cb = new utility::DeliveryReportCb();
    conf->set("dr_cb", ex_dr_cb, errstr);
    ::RdKafka::Conf *tconf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC);
    tconf->set("request.required.acks", "all", errstr);
    std::vector<std::string> topics = { "tp.test2" };

    producer = ::RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    topic = ::RdKafka::Topic::create(producer, topics[0], tconf, errstr);
    if (!topic) {
        std::cerr << ": Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "start producer \n";
    std::thread t([conf, tconf, ex_dr_cb]
    {
        while (producer_run) {
            producer->poll(1);
        }

        delete topic;
        delete producer;
        delete conf;
        delete tconf;
        delete ex_dr_cb;
    }
    );
    *tout = std::move(t);
}

int main()
{
    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::thread p_thread{};
    startProducer(&p_thread);
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });

    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("group.id", "xxx", errstr);
    conf->set("metadata.broker.list", brokers, errstr);

    utility::RebalanceCb ex_rebalance_cb{};
    conf->set("rebalance_cb", &ex_rebalance_cb, errstr);
    utility::ConsumeCb ex_consume_cb{};
    conf->set("consume_cb", &ex_consume_cb, errstr);

    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }
    delete conf;

    auto* p = RdKafka::TopicPartition::create("tp.test1", 0);
    consumer->assign({ p });
    delete p;

    while (consumer_run) {
        RdKafka::Message* msg = consumer->consume(1);
        auto str = utility::getMsgStr(*msg);
        auto offset = msg->offset();
        (void)offset;
        delete msg;

        if (!str.empty()) {
            ::RdKafka::ErrorCode resp = producer->produce(topic, 0, ::RdKafka::Producer::RK_MSG_COPY
                , const_cast<char*>(str.c_str()), str.size()
                , nullptr, nullptr);
            if (resp) {
                std::cout << "error send: " << ::RdKafka::err2str(resp) << "\n";
            }
            //if (!str.empty())
            //    std::cout << "recevie " << str << "  " << offset << "\n";
        }
    }
    consumer->close();
    delete consumer;
    producer_run = false;

    p_thread.join();
    ::RdKafka::wait_destroyed(5000);
    return 0;
}
