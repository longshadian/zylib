#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>
#include <chrono>
#include <ctime>

#include <rdkafkacpp.h>
#include "Utility.h"

static bool run = true;
static bool thread_run = true;
static void sigterm(int sig) {
    run = false;
}

class PKeyDeliveryReportCb : public ::RdKafka::DeliveryReportCb {
public:
    void dr_cb(::RdKafka::Message &message) {
        if (message.err()) {
            std::cout << "producer_send_error: " << message.errstr()
                << "    topic: " << message.topic_name()
                << "    key: " << utility::getKeyStr(message)
                << "    payload: " << utility::getMsgStr(message)
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
        }
    }
};

class ExampleEventCb : public ::RdKafka::EventCb {
public:
    void event_cb(::RdKafka::Event &event) {
        switch (event.type())
        {
        case ::RdKafka::Event::EVENT_ERROR:
            std::cerr << ": ERROR (" << ::RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
            break;

        case ::RdKafka::Event::EVENT_STATS:
            std::cerr << ": \"STATS\": " << event.str() << std::endl;
            break;

        case ::RdKafka::Event::EVENT_LOG:
            std::cerr << ": LOG-" << event.severity() << "-"
                << event.fac() << ": " << event.str() << std::endl;
            break;

        default:
            std::cerr << ": EVENT " << event.type() <<
                " (" << ::RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
            break;
        }
    }
};

int main() 
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    std::string topic_str;
    std::string topic_name = "tp.test1";
    int partition_number = 0;

    ::RdKafka::Conf *conf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL);
    ::RdKafka::Conf *tconf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    tconf->set("request.required.acks", "all", errstr);

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    PKeyDeliveryReportCb ex_dr_cb{};
    conf->set("dr_cb", &ex_dr_cb, errstr);

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

    std::string msg_str = "aaa";
    int32_t key = 0;
    while (run) {
        ++key;
        auto key_str = std::to_string(key);
        std::string msg = msg_str;
        ::RdKafka::ErrorCode resp = producer->produce(topic, partition_number
            , ::RdKafka::Producer::RK_MSG_COPY
            , const_cast<char*>(msg.c_str()), msg.size()
            , &key_str, nullptr);
        if (resp) {
            std::cout << "producer error " << resp << " " << ::RdKafka::err2str(resp) << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });
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
