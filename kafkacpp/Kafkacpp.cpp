#include "Kafkacpp.h"

namespace kafkacpp {

ProducerPtr createProduer(ConfUPtr conf)
{
    return std::make_shared<Producer>(std::move(conf));
}

ConsumerPtr createConsumer(ConfUPtr conf)
{
    return std::make_shared<Consumer>(std::move(conf));
}

} //kafkacpp
