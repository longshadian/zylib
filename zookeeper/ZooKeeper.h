#pragma once

#include <functional>

#include <zookeeper/zookeeper.h>

namespace zkcpp {

class ZooKeeper;
using WatcherCB = std::function<void(int type, int state, const char* path, ZooKeeper* zk)>;

std::string StateToString(int32_t state);
std::string TypeToString(int32_t type);

class ZooKeeper
{
public:
    ZooKeeper(zhandle_t* zk);
    ~ZooKeeper();
    ZooKeeper(const ZooKeeper& rhs) = delete;
    ZooKeeper& operator=(const ZooKeeper& rhs) = delete;
    ZooKeeper(ZooKeeper&& rhs) = delete;
    ZooKeeper& operator=(ZooKeeper&& rhs) = delete;

    //zhandle_t*              Init(std::string host, WatcherCB cb);
    zhandle_t*              GetHandle();

private:
    static void             WatcherCB_Wrapper(zhandle_t* zk, int type, int state, const char* path, void* ctx);
    void                    Destroy();

private:
    std::string             m_host;
    zhandle_t*              m_zk;
    WatcherCB               m_watch_cb;
};

} // zkcpp

