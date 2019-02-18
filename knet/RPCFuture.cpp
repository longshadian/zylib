#include "knet/RPCFuture.h"

#include "knet/UniformNetwork.h"

namespace knet {

RPCFuture::RPCFuture(UniformNetwork& network)
    : m_network(&network)
    , m_launch()
    , m_ctx()
{

}

RPCFuture::~RPCFuture()
{
    Launch();
}

RPCFuture::RPCFuture(RPCFuture&& rhs)
    : m_network(rhs.m_network)
    , m_launch(rhs.m_launch)
    , m_ctx(std::move(rhs.m_ctx))
{
}

RPCFuture& RPCFuture::operator=(RPCFuture&& rhs)
{
    if (this != &rhs) {
        std::swap(m_network, rhs.m_network);
        std::swap(m_launch, rhs.m_launch);
        std::swap(m_ctx, rhs.m_ctx);
    }
    return *this;
}

RPCFuture& RPCFuture::RPC(ServiceID sid, MsgID msg_id, MsgType msg)
{
    if (!m_pkg)
        m_pkg = std::make_unique<Pkg>();
    m_pkg->m_sid = std::move(sid);
    m_pkg->m_msg_id = msg_id;
    m_pkg->m_msg_type = std::move(msg);
    return *this;
}

void RPCFuture::Launch()
{
    if (m_launch)
        return;
    if (!m_pkg)
        return;
    m_network->RPC(std::move(m_pkg->m_sid)
        , m_pkg->m_msg_id, std::move(m_pkg->m_msg_type)
        , std::move(m_ctx));
    m_launch = true;
}

RPCFuture& RPCFuture::OnSuccess(RPCSuccessCB cb)
{
    auto& ctx = GetCtx();
    ctx.SetSuccessCB(std::move(cb));
    return *this;
}

RPCFuture& RPCFuture::OnTimeout(RPCTimeoutCB cb, Duration d)
{
    auto& ctx = GetCtx();
    ctx.SetTimeoutCB(std::move(cb), std::move(d));
    return *this;
}

RPCContext& RPCFuture::GetCtx()
{
    if (!m_ctx)
        m_ctx = std::make_unique<RPCContext>();
    return *m_ctx;
}

RPCFuture MakeFuture(UniformNetwork& network)
{
    return RPCFuture{network};
}

} // knet
