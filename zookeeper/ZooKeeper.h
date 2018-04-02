#pragma once

#include <functional>

#include <zookeeper.h>

namespace zk {
using WatcherCB = std::function<void(int, int, const char*, void*)>;

class ZooKeeper
{
public:
    ZooKeeper();
    ~ZooKeeper();
    ZooKeeper(const ZooKeeper& rhs) = delete;
    ZooKeeper& operator=(const ZooKeeper& rhs) = delete;
    ZooKeeper(ZooKeeper&& rhs) = delete;
    ZooKeeper& operator=(ZooKeeper&& rhs) = delete;

    bool Init(const std::string& host, WatcherCB cb);

private:
    static void WatcherCB_Wrapper(zhandle_t* zk, int type, int state, const char* path, void* watcherCtx);

    void destroy();

private:
    zhandle_t*      m_zk;
    WatcherCB       m_watch_cb;
}


} // zk
