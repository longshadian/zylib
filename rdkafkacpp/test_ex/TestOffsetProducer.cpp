#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>
#include <chrono>
#include <ctime>

#include "rdkafkacpp.h"

static bool run = true;
static bool thread_run = true;
static void sigterm(int sig) {
    run = false;
}

static int32_t data_size = 10;
static int32_t send_count = 10;
static int32_t send_dr_count = 0;
static std::chrono::system_clock::time_point tnow{};

std::string getMsgStr(const ::RdKafka::Message& msg)
{
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

class ExampleDeliveryReportCb : public ::RdKafka::DeliveryReportCb {
public:
    void dr_cb(::RdKafka::Message &message) {
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
            ++send_dr_count;
            if (send_dr_count == send_count) {
                auto tend = std::chrono::system_clock::now();
                auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tnow).count();
                std::cout << "cb len: " << data_size << " count: " << send_count
                    << " cost: " << delta << "ms\n";
            }
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

#define CHECK_SET(conf_ret) \
do { \
    if (conf_ret != ::RdKafka::Conf::CONF_OK) { \
        std::cerr << __LINE__ << ":error. " << conf_ret << " " << errstr <<  std::endl; \
        return 0; \
    } \
} while (0)

int main() 
{
    std::string brokers = "127.0.0.1:9092";
    std::string errstr;
    std::string topic_str;
    std::string mode;
    std::string debug;
    ::RdKafka::Conf::ConfResult conf_ret = ::RdKafka::Conf::CONF_OK;

    /*
    * Create configuration objects
    */
    ::RdKafka::Conf *conf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL);
    ::RdKafka::Conf *tconf = ::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC);

    conf_ret = conf->set("metadata.broker.list", brokers, errstr);
    CHECK_SET(conf_ret);
    //conf->set("enable.auto.commit", "true", errstr);
    //tconf->set("auto.offset.reset", "end", errstr);

    ExampleEventCb ex_event_cb;
    conf_ret = conf->set("event_cb", &ex_event_cb, errstr);
    CHECK_SET(conf_ret);

    conf_ret = tconf->set("request.required.acks", "all", errstr);
    CHECK_SET(conf_ret);

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    ExampleDeliveryReportCb ex_dr_cb{};
    conf_ret = conf->set("dr_cb", &ex_dr_cb, errstr);
    CHECK_SET(conf_ret);

    ::RdKafka::Producer* producer = ::RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    ::RdKafka::Topic* topic = ::RdKafka::Topic::create(producer, "test.1", tconf, errstr);
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

    std::string str_msg = {};
    tnow = std::chrono::system_clock::now();
    for (int i = 0; i != send_count; ++i) {
        str_msg = "aaaa   " + std::to_string(i);
        producer->produce(topic, 0, ::RdKafka::Producer::RK_MSG_COPY
            , const_cast<char*>(str_msg.c_str()), str_msg.size(), nullptr, nullptr);
    }
    std::cout << "send finish. count: " << send_count << std::endl;

    while (run) {
        if (false) {
            auto tnow = std::time(nullptr);
            std::string msg = std::to_string(tnow);
            ::RdKafka::ErrorCode resp = producer->produce(topic, 0, ::RdKafka::Producer::RK_MSG_COPY
                    , const_cast<char*>(msg.c_str()), msg.size()
                    , NULL, NULL);
            if (resp) {
                std::cout << "producer error " << resp << " " << ::RdKafka::err2str(resp) << "\n";
            }
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
