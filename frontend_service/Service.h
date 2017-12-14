#pragma once

#include <atomic>
#include <string>
#include <chrono>
#include <memory>

class World;

class Service
{
public:
    Service();
    ~Service();
	Service(const Service& rhs) = delete;
	Service& operator=(const Service& rhs) = delete;
    static Service& instance();

    bool                        start();
    void                        loop();
    void                        stop();
    World&                      GetWorld();
private:
    std::atomic<bool>           m_is_running;
    std::unique_ptr<World>      m_world;
};

Service&    GetService();
World&      GetWorld();

