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
#include "Utility.h"

namespace test_server_producer
{

static bool run = true;
static bool thread_run = true;
static void sigterm(int sig) {
    run = false;
}

int TestServerProducer() 
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    std::string topic_str;
    std::string topic_name = "tp.test1";
    int partition_number = 0;

    ::RdKafka::Conf *conf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL);
    ::RdKafka::Conf *tconf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    //ExampleEventCb ex_event_cb;
    //conf->set("event_cb", &ex_event_cb, errstr);

    tconf->set("request.required.acks", "all", errstr);

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    //PKeyDeliveryReportCb ex_dr_cb{};
    //conf->set("dr_cb", &ex_dr_cb, errstr);

    ::RdKafka::Producer* producer = ::RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    ::RdKafka::Topic* topic = ::RdKafka::Topic::create(producer, topic_name, tconf, errstr);
    if (!topic) {
        std::cerr << ": Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "start producer \n";

    std::thread t( [producer]
        {
            while (thread_run) {
                producer->poll(1000);
            }
        }
    );

    std::string msg_str{};
    msg_str.resize(10, 'a');
    time_t key = 0;
    while (run) {
        ++key;
        key = std::time(nullptr);
        auto key_str = std::to_string(key);
        std::string msg = msg_str;
        ::RdKafka::ErrorCode resp = producer->produce(topic, partition_number
            , ::RdKafka::Producer::RK_MSG_COPY
            , const_cast<char*>(msg.c_str()), msg.size()
            , &key_str, nullptr);
        if (resp) {
            std::cout << "producer error " << resp << " " << ::RdKafka::err2str(resp) << "\n";
        }
        std::cout << "send " << key << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
        /*
        if (key > 10) {
            std::cout << "key: " << key << "\n";
            break;
        }
        */
    }
    thread_run = false;
    t.join();

    delete topic;
    delete producer;
    delete conf;
    delete tconf;
    ::RdKafka::wait_destroyed(5000);
    return 0;
}

}

