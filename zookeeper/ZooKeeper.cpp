#include "ZooKeeper.h"

namespace zk {

ZooKeeper::ZooKeeper()
    : m_zk()
{
}

ZooKeeper::~ZooKeeper()
{

}

bool ZooKeeper::Init(const std::string& host, WatcherCB cb)
{
    if (m_zk)
        destroy();
    m_zk = ::zookeeper_init(host.c_str(), &ZooKeeper::WatcherCB_Wrapper, 0, nullptr, nullptr, 0);
}

void ZooKeeper::WatcherCB_Wrapper(zhandle_t* zk, int type, int state, const char* path, void* watcherCtx)
{

}

void ZooKeeper::destroy()
{
    if (m_zk)
        ::zookeeper_close(m_zk);
    m_zk = nullptr;
}

} // zk
