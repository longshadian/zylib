#include "knet/UniformNetwork.h"

#include <cstring>

#include "knet/CallbackManager.h"
#include "knet/RPCManager.h"
#include "knet/TimerManager.h"
#include "knet/EventManager.h"
#include "knet/Message.h"

#include "knet/detail/kafka/Consumer.h"
#include "knet/detail/kafka/Producer.h"
#include "knet/detail/kafka/Callback.h"

namespace knet {

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UniformNetwork::UniformNetwork()
{
}

UniformNetwork::~UniformNetwork()
{
}

bool UniformNetwork::Init()
{
    m_event_manager = std::make_unique<EventManager>();
    m_cb_mgr = std::make_unique<CallbackManager>();
    m_consumer = std::make_unique<detail::Consumer>();
    m_producer = std::make_unique<detail::Producer>();
    m_rpc_manager = std::make_unique<RPCManager>(*this);
    m_timer_manager = std::make_unique<TimerManager>(*m_event_manager);
    return false;
}

void UniformNetwork::Tick(DiffTime diff)
{
    (void)diff;
}

void UniformNetwork::ProcessMsg()
{
    decltype(m_received_msgs) temp{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(temp, m_received_msgs);
    }

    while (!temp.empty()) {
        MessagePtr msg = std::move(temp.front());
        temp.pop();
        m_cb_mgr->CallbackMsg(nullptr, std::move(msg));
    }
}

void UniformNetwork::DispatchMsg(MessagePtr msg)
{
    // ÊÇrpcÏûÏ¢
    if (msg->HasRPCKey()) {
        m_rpc_manager->OnReceivedMsg(msg);
    } else {
        m_cb_mgr->CallbackMsg(nullptr, std::move(msg));
    }
}

void UniformNetwork::receviedMsg(const void* p, size_t p_len, const void* key, size_t key_len)
{
    auto msg = std::make_shared<KMessage>();
    msg->PayloadParseFromBinary(p, p_len);
    msg->KeyParseFromBinary(key, key_len);

    std::lock_guard<std::mutex> lk(m_mtx);
    m_received_msgs.push(std::move(msg));
}

} // knet
