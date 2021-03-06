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
#include "knet/detail/kafka/KafkaTypes.h"

namespace knet {

namespace detail {

class SendMsg;

struct ProducerCB 
{
    EventCBUPtr          m_event_cb{};
    DeliveryReportCBUPtr m_dr_cb{};
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
    void Flush();
    void SendTo(std::shared_ptr<SendMsg> send_msg);

private:
    void StartPollThread();
    void StartSendThread();
    void SendMessageInternal(const SendMsg& msg);
    ::RdKafka::Topic* FindOrCreate(const ServiceID& to_sid);

private:
    std::unique_ptr<ProducerConf>               m_p_conf;
    std::thread                                 m_poll_thread;  // poll线程
    std::thread                                 m_send_thread;  // 发送线程
    std::queue<std::shared_ptr<SendMsg>>        m_queue;
    std::mutex                                  m_mtx;
    std::condition_variable                     m_cond;
    std::atomic<bool>                           m_run;
    std::unique_ptr<::RdKafka::Producer>        m_producer;
    std::unique_ptr<ProducerCB>                 m_producer_cb;
    std::unordered_map<ServiceID, std::unique_ptr<::RdKafka::Topic>> m_topics;
};

} // detail

} // knet
