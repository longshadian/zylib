#pragma once

#include <atomic>
#include <string>
#include <chrono>
#include <memory>

#include "knet/knet.h"

class SIDManager;
class World;
class StreamServer;

class Service
{
    Service();
public:
    ~Service();
	Service(const Service& rhs) = delete;
	Service& operator=(const Service& rhs) = delete;
	Service(Service&& rhs) = delete;
	Service& operator=(Service&& rhs) = delete;

    static Service& instance();

    bool                        Start();
    void                        Loop();
    void                        Stop();
    SIDManager&                 GetSIDManager();
    World&                      GetWorld();
    ::knet::UniformNetwork&     GetKNet();
private:
    std::atomic<bool>           m_is_running;
    std::unique_ptr<SIDManager> m_sid_mgr;
    std::unique_ptr<World>      m_world;
    std::unique_ptr<::knet::UniformNetwork> m_knet;
    std::unique_ptr<StreamServer> m_network; 
};

Service&                    GetService();
SIDManager&                 GetSIDManager();
World&                      GetWorld();
::knet::UniformNetwork&     GetKNet();

