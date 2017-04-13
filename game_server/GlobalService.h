#pragma once

#include <memory>

class DataBaseService;

class Global
{
    Global() = default;
    ~Global() = default;
public:
    static bool init();

    static DataBaseService& getDataBaseService(); 
private:
    static Global& instance();
private:
    std::shared_ptr<DataBaseService> m_database_service;
};
