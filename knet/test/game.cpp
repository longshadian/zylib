
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>

#include "knet/knet.h"

class ConsoleLog : public knet::LogStream
{
public:
    ConsoleLog() = default;
    virtual ~ConsoleLog() = default;

    virtual void flush(std::string str) override
    {
        str.push_back('\n');
        std::cout << str;
    }
};


int main()
{
    knet::initLog(std::make_unique<ConsoleLog>());

    auto c_conf = std::make_unique<knet::ConsumerConf>();
    c_conf->m_group_id = "knet";
    c_conf->m_broker_list = "127.0.0.1:9092";
    c_conf->m_topic = "k.game";
    c_conf->m_partition = 0;

    auto p_conf = std::make_unique<knet::ProducerConf>();
    p_conf->m_broker_list = "127.0.0.1:9092";

    auto p = std::make_shared<knet::UniformNetwork>();
    p->setConsuerConf(std::move(c_conf));
    p->setProducerConf(std::move(p_conf));

    if (!p->Init()) {
        FAKE_LOG(WARNING) << "init error";
        return 0;
    }

    auto tprevious = std::chrono::system_clock::now();
    auto tnow = tprevious;
    while (true) {
        FAKE_LOG(DEBUG) << "sleep...";
        std::this_thread::sleep_for(std::chrono::seconds{1});
        p->RPC("k.lobby", 100, "xxxxxxxx", std::make_unique<knet::RPCContext>());
        tnow = std::chrono::system_clock::now();
        auto delta = std::chrono::duration_cast<knet::DiffTime>(tnow - tprevious);
        p->Tick(delta);
        tprevious = tnow;
    }

    return 0;
}
