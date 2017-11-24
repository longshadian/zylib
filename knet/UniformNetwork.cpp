#include "knet/UniformNetwork.h"

#include "knet/CallbackManager.h"
#include "knet/RPCManager.h"

namespace knet {

KMessage::KMessage()
    : m_msg_id()
    , m_payload()
    , m_key()
{
}

KMessage::~KMessage()
{
}

bool KMessage::HasRPCKey() const
{
    return m_key != 0;
}

void KMessage::PayloadParseFromBinary(const void* p, size_t p_len)
{
    if (p && p_len > 0) {
        const auto* up = reinterpret_cast<const uint8_t*>(p);
        m_payload.assign(up, up + p_len);
    }
}

bool KMessage::KeyParseFromBinary(const void* key, size_t key_len)
{
    if (key && key_len) {
        std::memcpy(&m_key, key, key_len);
    }
    return key_len == sizeof(RPCKey);
}

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
    m_cb_mgr = std::make_unique<CallbackManager>();
    m_consumer = std::make_unique<Consumer>();
    m_producer = std::make_unique<Producer>();
    m_rpc_manager = std::make_unique<RPCManager>();
    m_timer_manager = std::make_unique<TimerManager>();

    return false;
}

void UniformNetwork::Tick(DiffTime diff)
{

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
