#include "knet/UniformNetwork.h"

#include "knet/CallbackManager.h"
#include "knet/RPCManager.h"

namespace knet {

UniformNetwork::UniformNetwork()
{
}

UniformNetwork::~UniformNetwork()
{
}

bool UniformNetwork::Init()
{
    return false;
}

void UniformNetwork::Tick(DiffTime diff)
{

}

void UniformNetwork::ProcessMsg()
{
    decltype(m_queue) temp{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(temp, m_queue);
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

} // knet
