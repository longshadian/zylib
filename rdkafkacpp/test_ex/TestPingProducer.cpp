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

static bool run = true;
static bool producer_run = true;
static bool consumer_run = true;
static void sigterm(int sig) {
    run = false;
}

static int32_t data_size = 100;
static std::chrono::system_clock::time_point tnow{};

std::vector<int> cost_list;
struct CostTM
{
    bool m_valid;
    std::chrono::system_clock::time_point m_tm;
};

CostTM tsend    = {false, std::chrono::system_clock::now()};
CostTM treceive = {false, std::chrono::system_clock::now()};

void startProducer()
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    ::RdKafka::Conf *conf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL);
    conf->set("group.id", "xxx", errstr);
    conf->set("metadata.broker.list", brokers, errstr);
    utility::DeliveryReportCb ex_dr_cb{};
    conf->set("dr_cb", &ex_dr_cb, errstr);
    ::RdKafka::Conf *tconf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC);
    tconf->set("request.required.acks", "all", errstr);
    std::vector<std::string> topics = { "tp.test1" };

    ::RdKafka::Producer* producer = ::RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    ::RdKafka::Topic* topic = ::RdKafka::Topic::create(producer, topics[0], tconf, errstr);
    if (!topic) {
        std::cerr << ": Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "start producer \n";
    std::thread t( [producer]
        {
            try {
                while (producer_run) {
                    producer->poll(1000);
                }
            }
            catch (std::exception e) {
                std::cout << "producer exception :" << e.what() << "\n";
             }
        }
    );

    std::string str_msg{};
    str_msg.resize(data_size, 'a');
    while (run) {
        if (tsend.m_valid && treceive.m_valid) {
            auto n = (int)std::chrono::duration_cast<std::chrono::microseconds>(treceive.m_tm - tsend.m_tm).count();
            cost_list.push_back(n);

            tsend.m_valid = false;
            treceive.m_valid = false;

            tsend.m_tm = std::chrono::system_clock::now();
            ::RdKafka::ErrorCode resp = producer->produce(topic, 0, ::RdKafka::Producer::RK_MSG_COPY
                    , const_cast<char*>(str_msg.c_str()), str_msg.size()
                    , nullptr, nullptr);
            if (resp) {
                std::cout << "error send: " << ::RdKafka::err2str(resp) << "\n";
            }
            std::cout << "send " << str_msg << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }

    t.join();

    delete topic;
    delete producer;
    delete conf;
    delete tconf;
}

void startConsumer()
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("group.id", "xxx", errstr);
    conf->set("metadata.broker.list", brokers, errstr);

    utility::RebalanceCb ex_rebalance_cb{};
    conf->set("rebalance_cb", &ex_rebalance_cb, errstr);
    utility::ConsumeCb ex_consume_cb;
    conf->set("consume_cb", &ex_consume_cb, errstr);

    std::vector<std::string> topics = { "tp.test2" };
    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }
    delete conf;

    auto* p = RdKafka::TopicPartition::create("tp.test2", 0);
    consumer->assign({ p });
    delete p;
    std::thread t([consumer]
        {
            try {
                while (consumer_run) {
                    RdKafka::Message* msg = consumer->consume(1000);
                    treceive.m_valid = true;
                    treceive.m_tm = std::chrono::system_clock::now();
                    std::cout << "receive \n";
                    delete msg;
                }
            } catch (std::exception e) {
                std::cout << "consumer exception : " <<  e.what() << "\n";
            }
        });
    consumer->close();
    delete consumer;

    t.join();
}

int main() 
{
    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::thread p_thread([] { startProducer(); });
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    std::thread c_thread([] { startConsumer(); });
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    while (run) {
        if (cost_list.size() > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });
    }

    producer_run = false;
    p_thread.join();

    consumer_run = false;
    c_thread.join();

    std::sort(cost_list.begin(), cost_list.end());
    auto total = std::accumulate(cost_list.begin(), cost_list.end(), 0);
    std::cout << "cost " << (total / cost_list.size()) << "\n";

    ::RdKafka::wait_destroyed(5000);
    return 0;
}
