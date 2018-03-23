#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>

#include <rdkafkacpp.h>
#include "Utility.h"

static bool run = true;
static long msg_cnt = 0;
static int64_t msg_bytes = 0;
static void sigterm(int sig) {
    run = false;
}

class CKeyRebalanceCb : public RdKafka::RebalanceCb {
private:
    static void part_list_print(const std::vector<RdKafka::TopicPartition*>&partitions) {
        for (unsigned int i = 0; i < partitions.size(); i++)
            std::cerr << partitions[i]->topic() <<
            "[" << partitions[i]->partition() << "], ";
        std::cerr << "\n";
    }

public:
    void rebalance_cb(RdKafka::KafkaConsumer *consumer,
        RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*> &partitions) {
        std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

        part_list_print(partitions);

        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
        } else {
            consumer->unassign();
        }
    }
};

void processMsg(RdKafka::Message* message, void* opaque) 
{
    switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        /* Real message */
    {
        msg_cnt++;
        msg_bytes += message->len();
        RdKafka::MessageTimestamp ts = message->timestamp();

        auto payload = utility::getMsgStr(*message);
        auto key = utility::getKeyStr(*message);
        auto ts_name = utility::getTmName(*message);

        std::cout << "payload_size: " << payload.size()
            << " offset: " << message->offset()
            << " ts: " << ts_name << " " << ts.timestamp
            << " key: " << key
            << "\n";

        /*
        std::cout << "payload: " << payload
            << " offset: " << message->offset()
            << " ts: " << ts_name << " " << ts.timestamp
            << " key: " << key
            << "\n";
            */
        break;
    }
    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        break;

    default:
        /* Errors */
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
    }
}

class CKeyConsumeCb : public RdKafka::ConsumeCb {
public:
    void consume_cb(RdKafka::Message &msg, void *opaque) {
        std::cout << "consumer cb\n";
        processMsg(&msg, opaque);
    }
};


void startConsumer(std::string brokers
    , std::string group_id
    , std::string topic_name
    , int32_t partition_number
    , std::thread* tout)
{
    std::string errstr{};

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    conf->set("group.id", group_id, errstr);
    conf->set("metadata.broker.list", brokers, errstr);

    CKeyRebalanceCb ex_rebalance_cb{};
    conf->set("rebalance_cb", &ex_rebalance_cb, errstr);

    CKeyConsumeCb ex_consume_cb{};
    conf->set("consume_cb", &ex_consume_cb, errstr);

    conf->set("default_topic_conf", tconf, errstr);
    delete tconf;

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }
    delete conf;

    auto* p = RdKafka::TopicPartition::create(topic_name, partition_number);
    auto err3 = consumer->assign({ p });
    if (err3) {
        std::cout << "consumer assign error " << RdKafka::err2str(err3) << "\n";
    }

    std::cout << "start consumer \n";

    std::thread t([consumer] 
    {
        while (run) {
            RdKafka::Message *msg = consumer->consume(1);
            processMsg(msg, NULL);
            delete msg;
        }
        consumer->close();
        delete consumer;
    });

    *tout = std::move(t);
}


int main() 
{
    std::string brokers = "127.0.0.1:9092";
    std::string topic_name = "tp.test1";
    int partition_number = 0;
    //std::string group_id = "xxx";

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::thread c1{};
    startConsumer(brokers, "xxx", topic_name, partition_number, &c1);

    //std::thread c2{};
    //startConsumer(brokers, "xxx2", topic_name, partition_number, &c2);

    c1.join();
    //c2.join();

    RdKafka::wait_destroyed(5000);

    return 0;
}
