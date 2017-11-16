#pragma once

#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

#include <rdkafkacpp.h>

#include "knet/KNetTypes.h"

namespace knet {

struct ClientCallback;
class Message;

struct ProducerConf
{
    std::string m_broker_list{};
};

class ProducerMsg
{
public:
    ProducerMsg();
    virtual ~ProducerMsg();

    ProducerMsg(const ProducerMsg&) = delete;
    ProducerMsg& operator=(const ProducerMsg&) = delete;
    ProducerMsg(ProducerMsg&&) = delete;
    ProducerMsg& operator=(ProducerMsg&&) = delete;

    virtual bool            Parse() { return true; }
    virtual const void*     GetPtr() const { return nullptr; }
    virtual size_t          GetSize() const { return 0; }
private:

};

struct ProducerCB 
{
    ReplayEventCBUPtr            m_event_cb{};
    ProducerDeliveryReportCBUPtr m_dr_cb{};
};

class Producer
{
public:
    Producer();
    ~Producer();
    Producer(const Producer&) = delete;
    Producer& operator=(const Producer&) = delete;
    Producer(Producer&&) = delete;
    Producer& operator=(Producer&&) = delete;

    bool Init(std::unique_ptr<ProducerConf> p_conf, std::unique_ptr<ProducerCB> client_cb);
    void Stop();
    void WaitThreadExit();
    void FlushReplay();
    void SendToMessage(std::string s);

private:
    void StartPollThread();
    void StartSendThread();
    void SendMessageInternal(const Message& msg);
    ::RdKafka::Topic* FindOrCreate(const ServiceID& sid);

private:
    std::unique_ptr<ProducerConf>               m_p_conf;
    std::thread                                 m_poll_thread;  // poll线程
    std::thread                                 m_send_thread;  // 发送线程
    std::queue<std::unique_ptr<ProducerMsg>>    m_queue;
    std::mutex                                  m_mtx;
    std::condition_variable                     m_cond;
    std::atomic<bool>                           m_run;
    std::unique_ptr<::RdKafka::Producer>        m_producer;
    std::unique_ptr<ProducerCB>                 m_producer_cb;
    std::unordered_map<ServiceID, std::unique_ptr<::RdKafka::Topic>> m_topics;
};

} // knet
