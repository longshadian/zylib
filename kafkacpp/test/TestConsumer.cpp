#include "Kafkacpp.h"

#include <memory>
#include <thread>
#include <iostream>

void cbReceivedMsg(rd_kafka_message_t* rkmessage)
{
    if (rkmessage->err) {
        if (rkmessage->rkt) {
            std::cout << "topic: " << ::rd_kafka_topic_name(rkmessage->rkt)
                << " partition: " << rkmessage->partition
                << " offset: " << rkmessage->offset
                << " err_str: " << ::rd_kafka_err2str(rkmessage->err)
                << " " << ::rd_kafka_message_errstr(rkmessage)
                << "\n";
        } else {
            std::cout << "topic: unknown "
                << " partition: " << rkmessage->partition
                << " offset: " << rkmessage->offset
                << " err_str: " << ::rd_kafka_err2str(rkmessage->err)
                << " " << ::rd_kafka_message_errstr(rkmessage)
                << "\n";
        }
        if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
            rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
            return;
    }

    char* pos = reinterpret_cast<char*>(rkmessage->payload);
    std::string payload{ pos, pos + rkmessage->len };

    pos = reinterpret_cast<char*>(rkmessage->key);
    std::string key{ pos, pos + rkmessage->key_len };

    std::cout << " " << payload << "  " << rkmessage->offset << "\n";

    //::rd_kafka_commit(m_rd_kafka, m_rd_kafka_topic_partition_list, 10);

    /*
    std::cout << "msg:  "
    << " topic: " << ::rd_kafka_topic_name(rkmessage->rkt)
    << " partition: " << rkmessage->partition
    << " offset: " << rkmessage->offset
    << " msg: " << payload << " len: " << rkmessage->len
    << " key: " << key << " key_len: " << rkmessage->key_len;
    */
}

void cbCommite(rd_kafka_t* rk, rd_kafka_resp_err_t err, rd_kafka_topic_partition_list_t* tp_list)
{
    std::cout << "commite xxxx\n";
}

void cbConsumer(rd_kafka_message_t* rk)
{
    std::cout << "cb_consumer \n";
}

int main()
{
    auto conf = kafkacpp::Conf::create();
    conf->set("group.id", "xxx");
    conf->set("bootstrap.servers", "127.0.0.1:9092");
    auto consumer = kafkacpp::createConsumer(std::move(conf));
    consumer->setCommiteCB(std::bind(&cbCommite, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    consumer->setReceivedMsgCB(std::bind(&cbReceivedMsg, std::placeholders::_1));
    consumer->setConsumerCB(std::bind(&cbConsumer, std::placeholders::_1));
    consumer->addTopicAndPartition("tp.test2", 0);
    if (!consumer->init()) {
        std::cout << "init fail\n";
        return 0;
    }

    //consumer->addBrokerList("127.0.0.1:9092");

    if (!consumer->startConsume()) {
        std::cout << "startConsume fail\n";
        return 0;
    }

    int64_t offset = 0;
    if (!consumer->getOffset("tp.test", 0, &offset)) {
        std::cout << "getOffset fail\n";
        std::cout << ::rd_kafka_err2str(::rd_kafka_last_error()) << "\n";
        return 0;
    }
    std::cout << "current offset: " << offset << "\n";
    //return 0;
    /*
    if (consumer->seekOffset("tp.test", 0, RD_KAFKA_OFFSET_END)) {
        std::cout << "seek offset success\n";
    } else {
        std::cout << "seek offset error\n";
    }
    */

    while (true) {
        consumer->poll(1000);
        //auto tnow = std::time(nullptr);
        //std::cout << "tm: " << tnow << "\n";
        //std::this_thread::sleep_for(std::chrono::seconds{ 2 });
    }

    return 0;
}
