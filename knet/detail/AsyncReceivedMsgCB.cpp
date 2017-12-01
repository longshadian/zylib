#include "knet/detail/AsyncReceivedMsgCB.h"

#include "knet/RPCManager.h"

namespace knet {

namespace detail {

AsyncReceivedMsgCB::AsyncReceivedMsgCB(RPCManager& rpc)
    : m_rpc(rpc)
{

}
    
AsyncReceivedMsgCB::~AsyncReceivedMsgCB()
{

}

void AsyncReceivedMsgCB::onError(int32_t err_no, const std::string& err_str)
{
    ReceiveMessageCB::onError(err_no, err_str);
}

void AsyncReceivedMsgCB::onReceived(const void* p, size_t p_len, const void* key, size_t key_len)
{
    m_rpc.CB_ReceviedMsg(p, p_len, key, key_len);
}

} // detail

} // knet

