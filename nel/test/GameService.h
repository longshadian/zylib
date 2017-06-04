#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <memory>

#include "UnifiedNetwork.h"

class GameService
{
public:
    GameService();
    ~GameService();
    GameService(const GameService& rhs) = delete;
    GameService& operator=(const GameService& rhs) = delete;
    GameService(GameService&& rhs) = delete;
    GameService& operator=(GameService&& rhs) = delete;

    bool start();
    bool sendMsg(std::string s);
    void update(uint32_t diff);

private:
    std::unique_ptr<NLNET::UnifiedNetwork> m_network;
};
