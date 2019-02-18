#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <memory>

#include "UnifiedNetwork.h"

class DataBaseService
{
public:
    DataBaseService();
    ~DataBaseService();
    DataBaseService(const DataBaseService& rhs) = delete;
    DataBaseService& operator=(const DataBaseService& rhs) = delete;
    DataBaseService(DataBaseService&& rhs) = delete;
    DataBaseService& operator=(DataBaseService&& rhs) = delete;

    bool start();
    bool sendMsg(std::string s);
    void update(uint32_t diff);

private:
    std::unique_ptr<nlnet::UnifiedNetwork> m_network;
};

