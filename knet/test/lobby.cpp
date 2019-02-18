
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

std::string toString(const void* p, size_t len)
{
    auto* pp = reinterpret_cast<const char*>(p);
    return std::string{pp, pp+len};
}

void fun100(knet::ReceivedMsgCtxPtr p1, knet::ReceivedMsgPtr p2)
{
    (void)p1;
    FAKE_LOG(DEBUG) << "key: " << p2->GetKey() 
        << " payload: " << p2->GetMsg();

    FAKE_LOG(DEBUG) << "key: " << p1->GetKey() 
        << " from: " << p1->GetFromSID()
        << " to: " << p1->GetToSID();
    p1->SendResponse(200, "send rsp");
}

int main()
{
    knet::MsgCallbackArray arr = 
    {
        { 100, std::bind(&fun100, std::placeholders::_1, std::placeholders::_2) }
    };

    knet::initLog(std::make_unique<ConsoleLog>());

    auto c_conf = std::make_unique<knet::ConsumerConf>();
    c_conf->m_group_id = "knet";
    c_conf->m_broker_list = "127.0.0.1:9092";
    c_conf->m_topic = "k.lobby";
    c_conf->m_partition = 0;

    auto p_conf = std::make_unique<knet::ProducerConf>();
    p_conf->m_broker_list = "127.0.0.1:9092";

    auto p = std::make_shared<knet::UniformNetwork>();
    p->SetConsuerConf(std::move(c_conf));
    p->SetProducerConf(std::move(p_conf));
    p->SetMsgCallbackArray(std::move(arr));

    if (!p->Init()) {
        FAKE_LOG(WARNING) << "init error";
        return 0;
    }

    auto tprevious = std::chrono::system_clock::now();
    auto tnow = tprevious;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds{1});
        FAKE_LOG(DEBUG) << "sleep...";
        tnow = std::chrono::system_clock::now();
        auto delta = std::chrono::duration_cast<knet::DiffTime>(tnow - tprevious);
        p->Tick(delta);
        tprevious = tnow;
    }

    return 0;
}
