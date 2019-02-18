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

static int32_t data_size = 200;
static std::chrono::system_clock::time_point tnow{};

std::vector<int> cost_list;
struct CostTM
{
    bool m_valid;
    std::chrono::system_clock::time_point m_tm;
};

CostTM tsend    = {false, std::chrono::system_clock::now()};
CostTM treceive = {false, std::chrono::system_clock::now()};

::RdKafka::Producer* producer;
::RdKafka::Topic* topic;

void startProducer(std::thread* tout)
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    ::RdKafka::Conf *conf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL);
    //conf->set("group.id", "xxx", errstr);
    conf->set("metadata.broker.list", brokers, errstr);
    utility::DeliveryReportCb* ex_dr_cb = new utility::DeliveryReportCb();
    conf->set("dr_cb", ex_dr_cb, errstr);
    ::RdKafka::Conf *tconf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC);
    tconf->set("request.required.acks", "all", errstr);
    std::vector<std::string> topics = { "tp.test1" };

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
    std::thread t( [conf, tconf, ex_dr_cb]
        {
            try {
                while (producer_run) {
                    producer->poll(1);
                }
            } catch (std::exception e) {
                std::cout << "producer exception :" << e.what() << "\n";
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

void startConsumer(std::thread* tout)
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("group.id", "xxx", errstr);
    conf->set("metadata.broker.list", brokers, errstr);

    /*
    conf->set("batch.num.messages", "1", errstr);
    if (!errstr.empty())
        std::cout << "batch.num.messages " << errstr << "\n";
    conf->set("queue.buffering.max.ms", "0", errstr);
    if (!errstr.empty())
        std::cout << "queue.buffering.max.ms " << errstr << "\n";
        */

    utility::RebalanceCb* ex_rebalance_cb = new utility::RebalanceCb();
    conf->set("rebalance_cb", ex_rebalance_cb, errstr);

    utility::ConsumeCb* ex_consume_cb = new utility::ConsumeCb();
    conf->set("consume_cb", ex_consume_cb, errstr);

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
    std::thread t([consumer, ex_rebalance_cb, ex_consume_cb]
        {
            try {
                while (consumer_run) {
                    RdKafka::Message* msg = consumer->consume(1);
                    auto str = utility::getMsgStr(*msg);
                    if (!str.empty()) {
                        treceive.m_tm = std::chrono::system_clock::now();
                        treceive.m_valid = true;
                        //std::cout << "receive " << str << "\n";
                    }
                    delete msg;
                }
            } catch (std::exception e) {
                std::cout << "consumer exception : " <<  e.what() << "\n";
            }

            consumer->close();
            delete consumer;
            delete ex_rebalance_cb;
            delete ex_consume_cb;
        });
    *tout = std::move(t);
}

int main() 
{
    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::thread p_thread{};
    startProducer(&p_thread);
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });

    std::thread c_thread{};
    startConsumer(&c_thread);
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });

    std::string str_msg{};
    str_msg.resize(data_size, 'a');

    {
        tsend.m_tm = std::chrono::system_clock::now();
        ::RdKafka::ErrorCode err_no = producer->produce(topic, 0, ::RdKafka::Producer::RK_MSG_COPY
                , const_cast<char*>(str_msg.c_str()), str_msg.size()
                , nullptr, nullptr);
        if (err_no) {
            tsend.m_valid = true;
            std::cout << "error send: " << ::RdKafka::err2str(err_no) << "\n";
        }
        //std::cout << "send " << str_msg << "\n";
    }
    while (run) {
        if (treceive.m_valid) {
            auto n = (int)std::chrono::duration_cast<std::chrono::microseconds>(treceive.m_tm - tsend.m_tm).count();
            cost_list.push_back(n);

            tsend.m_valid = false;
            treceive.m_valid = false;

            tsend.m_tm = std::chrono::system_clock::now();
            ::RdKafka::ErrorCode err_no = producer->produce(topic, 0, ::RdKafka::Producer::RK_MSG_COPY
                    , const_cast<char*>(str_msg.c_str()), str_msg.size()
                    , nullptr, nullptr);
            if (err_no) {
                std::cout << "error send: " << ::RdKafka::err2str(err_no) << "\n";
            }
            //std::cout << "send " << str_msg << "\n";
        }

        if (cost_list.size() > 1000) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{10});
        //std::this_thread::sleep_for(std::chrono::seconds{ 2 });
    }

    producer_run = false;
    p_thread.join();

    consumer_run = false;
    c_thread.join();

    std::sort(cost_list.begin(), cost_list.end());
    auto total = std::accumulate(cost_list.begin(), cost_list.end(), 0);
    if (!cost_list.empty())
        std::cout << "cost " << (total / cost_list.size()) << "\n";
    else
        std::cout << "cost " << 0 << "\n";

    ::RdKafka::wait_destroyed(5000);
    return 0;
}
