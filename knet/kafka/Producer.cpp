#include "knet/kafka/Producer.h"

#include <functional>

#include "knet/kafka/Callback.h"

namespace knet {

Producer::Producer()
    : m_p_conf()
    , m_poll_thread()
    , m_send_thread()
    , m_run()
    , m_producer()
    , m_producer_cb(std::make_unique<ProducerCB>())
{
    m_producer_cb->m_event_cb = std::make_unique<EventCB>();
    m_producer_cb->m_dr_cb = std::make_unique<ProducerDeliveryReportCB>();
}

Producer::~Producer()
{
    Stop();
    WaitThreadExit();
}

bool Producer::Init(std::unique_ptr<ProducerConf> p_conf, std::unique_ptr<ProducerCB> client_cb)
{
    m_p_conf = std::move(p_conf);
    if (client_cb) {
        if (client_cb->m_dr_cb)
            m_producer_cb->m_dr_cb = std::move(client_cb->m_dr_cb);
        if (m_producer_cb->m_event_cb)
            m_producer_cb->m_event_cb = std::move(client_cb->m_event_cb);
    }

    std::unique_ptr<::RdKafka::Conf> conf{::RdKafka::Conf::create(::RdKafka::Conf::CONF_GLOBAL)};
    std::string err_str{};
    ::RdKafka::Conf::ConfResult conf_ret = ::RdKafka::Conf::CONF_OK;
    conf_ret = conf->set("metadata.broker.list", m_p_conf->m_broker_list, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        return false;
    }

    conf_ret = conf->set("event_cb", &*m_producer_cb->m_event_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        LOG(WARNING) << "conf_set event_cb fail. " << err_str;
        return false;
    }

    conf_ret = conf->set("dr_cb", &*m_producer_cb->m_dr_cb, err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        LOG(WARNING) << "conf_set dr_cb fail. " << err_str;
        return false;
    }

    std::unique_ptr<::RdKafka::Producer> producer{::RdKafka::Producer::create(&*conf, err_str)};
    if (!producer) {
        LOG(WARNING) << "create producer fail.";
        return false;
    }

    m_producer = std::move(producer);
    m_run = true;
    std::thread t{std::bind(&Producer::StartPollThread, this)};
    m_poll_thread = std::move(t);

    std::thread t2{std::bind(&Producer::StartSendThread, this)};
    m_send_thread = std::move(t2);
    return true;
}

void Producer::Stop()
{
    m_run = false;
}

void Producer::WaitThreadExit()
{
    if (m_poll_thread.joinable())
        m_poll_thread.join();
    if (m_send_thread.joinable())
        m_send_thread.join();
}

void Producer::FlushReplay()
{
    if (m_run)
        return;
    if (m_queue.empty())
        return;

    // 启动线程执行poll操作
    std::atomic<bool> run_ex{true};
    std::thread t{[this, &run_ex]
        {
            while (run_ex) {
                m_producer->poll(1000);
            }
        }
    };

    // 发送剩余录像
    while (!m_queue.empty()) {
        std::unique_ptr<Message> msg = std::move(m_queue.front());
        m_queue.pop();
        SendMessageInternal(*msg);
    }
    run_ex = false;
    t.join();

    ::RdKafka::ErrorCode ec = ::RdKafka::ERR_NO_ERROR;
    do {
        ec = m_producer->flush(2000);
    } while (ec != ::RdKafka::ERR_NO_ERROR);
}

void Producer::SendToMessage(std::string msg)
{
    if (!m_run)
        return;
    std::lock_guard<std::mutex> lk{m_mtx};
    m_queue.push(std::move(msg));
    m_cond.notify_one();
}

void Producer::StartPollThread()
{
    while (m_run) {
        m_producer->poll(1000);
    }
}

void Producer::StartSendThread()
{
    std::unique_ptr<Message> msg{};
    while (m_run) {
        msg = nullptr;
        {
            std::unique_lock<std::mutex> lk{m_mtx};
            m_cond.wait_for(lk, std::chrono::seconds{1}, [this] { return !m_queue.empty(); });
            if (!m_queue.empty()) {
                msg = std::move(m_queue.front());
                m_queue.pop();
            }
        }
        if (msg) {
            SendMessageInternal(*msg);
        }
    }
}

void Producer::SendMessageInternal(const Message& msg)
{
    std::pair<uint8_t*, int> buffer{nullptr, 0};
    auto ret = MessageDecoder::encoder(msg, &buffer);
    if (!ret) {
        LOG(WARNING) << "message encode fail. replay_id: " << msg.getReplayID() 
            << " sequence_id: " << msg.getSequenceID();
        return;
    }

    ::RdKafka::ErrorCode resp = m_producer->produce(&*m_topic
        , m_p_conf->m_partition, ::RdKafka::Producer::RK_MSG_FREE
        , buffer.first, buffer.second
        , nullptr, nullptr);
    if (resp) {
        LOG(WARNING) << "send replay fail. err: " << resp
            << " err_str: " << ::RdKafka::err2str(resp);
    }
    if (resp == -1) {
        ::free(buffer.first);
    }

}

::RdKafka::Topic* Producer::FindOrCreate(const ServiceID& sid)
{
    auto it = m_topics.find(sid);
    if (it != m_topics.end())
        return it->second.get();

    // 创建一个topic
    std::string err_str{};
    std::unique_ptr<::RdKafka::Conf> tconf{::RdKafka::Conf::create(::RdKafka::Conf::CONF_TOPIC)};
    ::RdKafka::Conf::ConfResult conf_ret = tconf->set("request.required.acks", "all", err_str);
    if (conf_ret != ::RdKafka::Conf::CONF_OK) {
        LOG(WARNING) << "conf_set request.required.acks fail. " << err_str;
        return nullptr;
    }

    std::unique_ptr<::RdKafka::Topic> new_topic{
        ::RdKafka::Topic::create(&*m_producer, sid, &*tconf, err_str)};
    if (!new_topic) {
        LOG(WARNING) << "create topic fail.";
        return nullptr;
    }

    auto* ptr = new_topic.get();
    m_topics.insert({sid, std::move(new_topic)});
    return ptr;
}

} // knet
