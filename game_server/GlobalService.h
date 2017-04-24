#pragma once

#include <memory>

class DataBaseService;
class NetworkService;
class World;

class Global
{
    Global() = default;
    ~Global() = default;
public:
    static bool init();

    static DataBaseService& getDataBaseService(); 
    static NetworkService& getNetworkService();
    static World& getWorld();
private:
    static Global& instance();
private:
    std::shared_ptr<DataBaseService> m_database_service;
    std::shared_ptr<NetworkService>  m_network_service;
    std::shared_ptr<World>           m_world;
};
