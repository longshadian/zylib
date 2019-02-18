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
    if (m_rd_kafka_conf)
        ::rd_kafka_conf_destroy(m_rd_kafka_conf);
    m_rd_kafka_conf = nullptr;
}

void Conf::reset()
{
    m_rd_kafka_conf = nullptr;
}

rd_kafka_conf_t* Conf::rd_kafka_conf()
{
    return m_rd_kafka_conf;
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

ConfUPtr Conf::create()
{
    auto* conf = new Conf();
    return std::unique_ptr<Conf>(conf);
}

} //kafkacpp
