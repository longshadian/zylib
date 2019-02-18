#pragma once

#include <array>

#include <rdkafka.h>

#include "Conf.h"
#include "Consumer.h"
#include "Producer.h"
#include "Topic.h"

namespace kafkacpp {

struct ErrStr
{
    std::array<char, 512> m_data;
    char* getPtr() { return m_data.data(); }
    size_t getLen() const { return m_data.size(); }
};

ProducerPtr createProduer(ConfUPtr conf);
ConsumerPtr createConsumer(ConfUPtr conf);

} //kafkacpp
