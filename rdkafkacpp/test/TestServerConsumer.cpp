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

class CEventCb : public RdKafka::EventCb {
public:
    virtual void event_cb(RdKafka::Event &event) override
    {
        switch (event.type())
        {
        case RdKafka::Event::EVENT_ERROR:
            std::cerr << __LINE__ << " ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
            if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                run = false;
            break;

        case RdKafka::Event::EVENT_STATS:
            std::cerr << __LINE__ << "  \"STATS\": " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_LOG:
            fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
            break;

        case RdKafka::Event::EVENT_THROTTLE:
            std::cerr << __LINE__ << "   THROTTLED: " << event.throttle_time() << "ms by " <<
                event.broker_name() << " id " << (int)event.broker_id() << std::endl;
            break;

        default:
            std::cerr << __LINE__ << "  EVENT " << event.type() <<
                " (" << RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
            break;
        }
    }
};


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

        std::cout << "payload: " << payload
            << " offset: " << message->offset()
            << " ts: " << ts_name << " " << ts.timestamp
            << " key: " << key
            << "\n";
        break;
    }
    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        std::cout << "eof\n";
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


class COffsetCommitCB : public RdKafka::OffsetCommitCb 
{
public:
    virtual void offset_commit_cb(RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*>&offsets) override
    {
        if (err == RdKafka::ERR_NO_ERROR) {
            for (RdKafka::TopicPartition* tp : offsets) {
                std::cout << "offset_commit_cb: " << RdKafka::err2str(tp->err())
                    << " tp_name: " << tp->topic()
                    << " partition: " << tp->partition()
                    << " offset: " << tp->offset()
                    << "\n";
            }
        }
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
    //conf->set("enable.auto.commit", "false", errstr);

    //CKeyRebalanceCb ex_rebalance_cb{};
    //conf->set("rebalance_cb", &ex_rebalance_cb, errstr);

    //CKeyConsumeCb ex_consume_cb{};
    //conf->set("consume_cb", &ex_consume_cb, errstr);

    COffsetCommitCB* ex_offset_cb = new COffsetCommitCB();
    conf->set("offset_commit_cb", ex_offset_cb, errstr);
    if (!errstr.empty()) {
        std::cout << "offset_commit_cb err: " << errstr << "\n";
    }

    errstr.clear();
    CEventCb* ex_event_cb = new CEventCb();
    conf->set("event_cb", ex_event_cb, errstr);
    if (!errstr.empty()) {
        std::cout << "event_cb err: " << errstr << "\n";
    }

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
    //p->set_offset(RdKafka::Topic::OFFSET_STORED);
    p->set_offset(RdKafka::Topic::OFFSET_END);
    
    auto err3 = consumer->assign({ p });
    if (err3) {
        std::cout << "consumer assign error " << RdKafka::err2str(err3) << "\n";
    }

    //int ev_n = consumer->poll(1000);
    int ev_n = 0;
    std::cout << " event_n: " << ev_n << "\n";

    auto err4 = consumer->seek(*p, 1000);
    if (err4) {
        std::cout << "consumer seek error " << err4 
            << "    " << RdKafka::err2str(err4) 
            << " event_n: " << ev_n
            << "\n";
    }

    std::vector<RdKafka::TopicPartition*> out_p = {p};
    auto err5 = consumer->position(out_p);
    if (err5) {
        std::cout << "consumer position error " << err5 
            << "    " << RdKafka::err2str(err5) 
            << "\n";
    }
    std::cout << "current offset: " << p->offset() << "\n";

    std::cout << "start consumer \n";

    std::thread t([consumer, ex_offset_cb]
    {
        while (run) {
            RdKafka::Message *msg = consumer->consume(1);
            processMsg(msg, NULL);
            consumer->commitSync(msg);
            delete msg;
        }
        consumer->close();
        delete consumer;
        delete ex_offset_cb;
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

    c1.join();
    RdKafka::wait_destroyed(5000);

    return 0;
}
