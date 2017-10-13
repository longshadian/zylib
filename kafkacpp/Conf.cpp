#include "Kafkacpp.h"

#include <memory>
#include <array>

#include <rdkafka.h>

namespace kafkacpp {

Conf::Conf() 
    : m_rd_kafka_conf(::rd_kafka_conf_new())
{
}

Conf::~Conf()
{
    // TODO
    /*
    if (m_rd_kafka_conf)
        ::rd_kafka_conf_destroy(m_rd_kafka_conf);
        */
}

rd_kafka_conf_t* Conf::rd_kafka_conf()
{
    return m_rd_kafka_conf;
}

ProducerPtr Conf::createProduer(std::string* str)
{
    ErrStr err_str{};
    ::rd_kafka_conf_set_dr_msg_cb(rd_kafka_conf(), &Producer::cbDrMsg);
    auto rk = ::rd_kafka_new(RD_KAFKA_PRODUCER, rd_kafka_conf(), err_str.getPtr(), err_str.getLen());
    if (str)
        *str = err_str.getPtr();
    if (rk) {
        return std::make_shared<Producer>(rk);
    }
    return nullptr;
}

ConsumerPtr Conf::createConsumer(std::string* str)
{
    ErrStr err_str{};
    ::rd_kafka_conf_set_consume_cb(rd_kafka_conf(), &Consumer::cbConsume);
    auto rk = ::rd_kafka_new(RD_KAFKA_CONSUMER, rd_kafka_conf(), err_str.getPtr(), err_str.getLen());
    if (str)
        *str = err_str.getPtr();
    if (rk) {
        ::rd_kafka_brokers_add(rk, "127.0.0.1:9092");
        return std::make_shared<Consumer>(rk);
    }
    return nullptr;
}

bool Conf::set(const char* name, const char* value, std::string* str, rd_kafka_conf_res_t* val)
{
    ErrStr err_str{};
    rd_kafka_conf_res_t ret = ::rd_kafka_conf_set(m_rd_kafka_conf
        , name, value
        , err_str.getPtr(), err_str.getLen());
    if (str)
        *str = err_str.getPtr();
    if (val)
        *val = ret;
    return ret == RD_KAFKA_CONF_OK;
}

} //kafkacpp
