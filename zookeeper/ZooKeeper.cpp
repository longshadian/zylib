#include "ZooKeeper.h"

namespace zkcpp {
std::string StateToString(int32_t state)
{
    if (state == ZOO_EXPIRED_SESSION_STATE)
        return "ZOO_EXPIRED_SESSION_STATE";
    else if (state == ZOO_AUTH_FAILED_STATE)
        return "ZOO_AUTH_FAILED_STATE";
    else if (state == ZOO_CONNECTING_STATE)
        return "ZOO_CONNECTING_STATE";
    else if (state == ZOO_ASSOCIATING_STATE)
        return "ZOO_ASSOCIATING_STATE";
    else if (state == ZOO_CONNECTED_STATE)
        return "ZOO_CONNECTED_STATE";
    return "unknown state";
}

std::string TypeToString(int32_t type)
{
    if (type == ZOO_CREATED_EVENT)
        return "ZOO_CREATED_EVENT";
    else if (type == ZOO_DELETED_EVENT)
        return "ZOO_DELETED_EVENT";
    else if (type == ZOO_CHANGED_EVENT)
        return "ZOO_CHANGED_EVENT";
    else if (type == ZOO_CHILD_EVENT)
        return "ZOO_CHILD_EVENT";
    else if (type == ZOO_SESSION_EVENT)
        return "ZOO_SESSION_EVENT";
    else if (type == ZOO_NOTWATCHING_EVENT)
        return "ZOO_NOTWATCHING_EVENT";
    return "unknown type";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ZooKeeper::ZooKeeper(zhandle_t* zk)
    : m_host()
    , m_zk(zk)
    , m_watch_cb()
{
}

ZooKeeper::~ZooKeeper()
{
    if (m_zk)
        Destroy();
}

/*
zhandle_t* ZooKeeper::Init(std::string host, WatcherCB cb)
{
    int32_t timeout = 30000;
    if (m_zk)
        Destroy();
    m_host = std::move(host);
    m_zk = ::zookeeper_init(m_host.c_str(), &ZooKeeper::WatcherCB_Wrapper, timeout, nullptr, this, 0);
    return GetZHandle();
}
*/

zhandle_t* ZooKeeper::GetHandle()
{
    return m_zk;
}

void ZooKeeper::WatcherCB_Wrapper(zhandle_t* zk, int type, int state, const char* path, void* ctx)
{
    ZooKeeper* p_this = reinterpret_cast<ZooKeeper*>(ctx);
    p_this->m_watch_cb(type, state, path, p_this);
}

void ZooKeeper::Destroy()
{
    ::zookeeper_close(m_zk);
    m_zk = nullptr;
}

} // zkcpp
