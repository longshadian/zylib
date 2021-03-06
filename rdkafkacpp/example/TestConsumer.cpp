#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <librdkafka/rdkafkacpp.h>

#include "Log.h"

namespace test_consumer 
{

static bool run = true;
static bool exit_eof = false;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;
static long msg_cnt = 0;
static int64_t msg_bytes = 0;
static void sigterm(int sig) {
    run = false;
}

class ExampleRebalanceCb : public RdKafka::RebalanceCb {
private:
    static void part_list_print(const std::vector<RdKafka::TopicPartition*>&partitions) {
        for (unsigned int i = 0; i < partitions.size(); i++) {
            LOG_WARN("index: %d %s %d", i, partitions[i]->topic().c_str(), partitions[i]->partition());
        }
    }

public:
    void rebalance_cb(RdKafka::KafkaConsumer *consumer,
        RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*> &partitions) {
        std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

        part_list_print(partitions);

        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
            partition_cnt = (int)partitions.size();
        }
        else {
            consumer->unassign();
            partition_cnt = 0;
        }
        eof_cnt = 0;
    }
};

void msg_consume(RdKafka::Message* message, void* opaque) {
    switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        /* Real message */
        msg_cnt++;
        msg_bytes += message->len();
        if (verbosity >= 3)
            std::cerr << "Read msg at offset " << message->offset() << std::endl;
        RdKafka::MessageTimestamp ts;
        ts = message->timestamp();
        if (verbosity >= 2 &&
            ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
            std::string tsname = "?";
            if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
                tsname = "create time";
            else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                tsname = "log append time";
            std::cout << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
        }
        if (verbosity >= 2 && message->key()) {
            std::cout << "Key: " << *message->key() << std::endl;
        }
        if (verbosity >= 1) {
            //std::cout << (const char*)message->payload() << " offset " << message->offset() << "\n";
            LOG_DEBUG("%.*s", static_cast<int>(message->len()), static_cast<const char *>(message->payload()));
        }
        break;

    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        if (exit_eof && ++eof_cnt == partition_cnt) {
            std::cerr << "%% EOF reached for all " << partition_cnt <<
                " partition(s)" << std::endl;
            run = false;
        }
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        run = false;
        break;

    default:
        /* Errors */
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        run = false;
    }
}

class ExampleConsumeCb : public RdKafka::ConsumeCb {
public:
    void consume_cb(RdKafka::Message &msg, void *opaque) {
        std::cout << "consumer cb\n";
        msg_consume(&msg, opaque);
    }
};

int TestConsumer() 
{
    std::string brokers = "192.168.97.226:9092";
    std::string errstr;
    std::string topic_str;
    std::string mode;
    std::string debug;

    /*
    * Create configuration objects
    */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    conf->set("group.id", "group.id.test", errstr);

    ExampleRebalanceCb ex_rebalance_cb;
    conf->set("rebalance_cb", &ex_rebalance_cb, errstr);
    //std::vector<std::string> topics = { "test" };

    /*
    * Set configuration properties
    */
    conf->set("metadata.broker.list", brokers, errstr);
    //conf->set("enable.auto.commit", "true", errstr);
    tconf->set("auto.offset.reset", "latest", errstr);

    ExampleConsumeCb ex_consume_cb;
    conf->set("consume_cb", &ex_consume_cb, errstr);

    //ExampleEventCb ex_event_cb;
    //conf->set("event_cb", &ex_event_cb, errstr);
    conf->set("default_topic_conf", tconf, errstr);
    delete tconf;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    /*
    * Create consumer using accumulated global configuration.
    */
    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    delete conf;

    std::cout << "% Created consumer " << consumer->name() << std::endl;
    auto* p = RdKafka::TopicPartition::create("test", 0); //RdKafka::Topic::PARTITION_UA
        //, RdKafka::Topic::OFFSET_END);
    auto err3 = consumer->assign({ p });
    if (err3) {
        std::cout << "consumer assign error " << RdKafka::err2str(err3) << "\n";
    }

    /*
    consumer->poll(100);
    p->set_offset(RdKafka::Topic::OFFSET_END);
    RdKafka::ErrorCode err2 = consumer->seek(*p, 5000);
    if (err2) {
        std::cout << "seek error " << RdKafka::err2str(err2) << "\n";
    }
    delete p;
    */

    /*
    RdKafka::ErrorCode err = consumer->subscribe(topics);
    if (err) {
        std::cerr << "Failed to subscribe to " << topics.size() << " topics: "
            << RdKafka::err2str(err) << std::endl;
        exit(1);
    }
    */

    std::cout << "start consumer \n";

    /*
    * Consume messages
    */
    while (run) {
        RdKafka::Message *msg = consumer->consume(1000);
        msg_consume(msg, NULL);
        delete msg;
    }

    /*
    * Stop consumer
    */
    consumer->close();
    delete consumer;

    //std::cerr << "% Consumed " << msg_cnt << " messages (" << msg_bytes << " bytes)" << std::endl;

    /*
    * Wait for RdKafka to decommission.
    * This is not strictly needed (with check outq_len() above), but
    * allows RdKafka to clean up all its resources before the application
    * exits so that memory profilers such as valgrind wont complain about
    * memory leaks.
    */
    RdKafka::wait_destroyed(5000);

    return 0;
}

}
