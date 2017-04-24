#include "GlobalService.h"

#include "zylib/Tools.h"
#include "Log.h"
#include "NetworkService.h"
#include "World.h"

bool Global::init()
{
    auto network_service = std::make_shared<NetworkService>();
    if (!network_service->init()) {
        return false;
    }
    instance().m_network_service = std::move(network_service);

    auto world = std::make_shared<World>();
    if (!world->init()) {
        return false;
    }
    instance().m_world = std::move(world);

    return true;
}

Global& Global::instance()
{
    static Global instance_;
    return instance_;
}

NetworkService& Global::getNetworkService()
{
    return *instance().m_network_service;
}

World& Global::getWorld()
{
    return *instance().m_world;
}

