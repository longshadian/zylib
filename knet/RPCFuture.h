#pragma once

#include "knet/KNetTypes.h"
#include "knet/RPCManager.h"

namespace knet {

class UniformNetwork;

class RPCFuture
{
    struct Pkg
    {
        ServiceID       m_sid{};
        MsgID           m_msg_id{};
        MsgType         m_msg_type{};
    };

public:
    RPCFuture(UniformNetwork& network);
    ~RPCFuture();
    RPCFuture(const RPCFuture& rhs) = delete;
    RPCFuture& operator=(const RPCFuture& rhs) = delete;
    RPCFuture(RPCFuture&& rhs);
    RPCFuture& operator=(RPCFuture&& rhs);

    RPCFuture&              RPC(ServiceID sid, MsgID msg_id, MsgType msg);
    RPCFuture&              OnSuccess(RPCSuccessCB cb);
    RPCFuture&              OnTimeout(RPCTimeoutCB cb, Duration d);
    void                    Launch();
private:
    RPCContext&             GetCtx();
private:
    UniformNetwork*         m_network;
    bool                    m_launch;
    RPCContextUPtr          m_ctx;
    std::unique_ptr<Pkg>    m_pkg;
};

RPCFuture MakeFuture(UniformNetwork& network);

} // knet
