#include "knet/detail/AsyncReceivedMsgCB.h"

#include "knet/UniformNetwork.h"

namespace knet {

namespace detail {

AsyncReceivedMsgCB::AsyncReceivedMsgCB(UniformNetwork& un)
    : m_un(un)
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
    m_un.ReceviedMsg_CB(p, p_len, key, key_len);
}

} // detail

} // knet

