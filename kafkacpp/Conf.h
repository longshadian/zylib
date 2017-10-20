#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <unordered_map>
#include <array>
#include <functional>

#include <rdkafka.h>

namespace kafkacpp {

class Conf;
using ConfUPtr = std::unique_ptr<Conf>;

class Conf 
{
    Conf();
public:
    ~Conf();
    Conf(const Conf& rhs) = delete;
    Conf& operator=(const Conf& rhs) = delete;
    Conf(Conf&& rhs) = delete;
    Conf& operator=(Conf&& rhs) = delete;

    void reset();

    rd_kafka_conf_t* rd_kafka_conf();
    bool set(const char* name, const char* value, std::string* str = nullptr, rd_kafka_conf_res_t* val = nullptr);

    static ConfUPtr create();
private:
    rd_kafka_conf_t* m_rd_kafka_conf;
};

} //kafkacpp
