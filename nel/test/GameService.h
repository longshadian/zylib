#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <memory>

#include "UnifiedNetwork.h"
#include "Types.h"
#include "TSock.h"

class GSCallbackManager;

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

    GSCallbackManager& getCBMgr();

    std::unique_ptr<nlnet::UnifiedNetwork> m_network;
    std::unique_ptr<GSCallbackManager>     m_gs_cb_mgr;
private:
    void cbServiceUp(nlnet::TSockContext& sock);
    void cbServiceDown(nlnet::TSockContext& sock);
};
