#pragma once

#include <memory>
#include <mutex>
#include <queue>

#include "knet/KNetTypes.h"
#include "knet/detail/kafka/Callback.h"

namespace knet {

class RPCManager;

namespace detail {

class AsyncReceivedMsgCB : public ReceiveMessageCB
{
public:
    AsyncReceivedMsgCB(RPCManager& rpc);
    virtual ~AsyncReceivedMsgCB();

    virtual void onError(int32_t err_no, const std::string& err_str) override;
    virtual void onReceived(const void* p, size_t p_len, const void* key, size_t key_len) override;

private:
    RPCManager& m_rpc;
};

} // detail

} // knet

