#include "GameService.h"

#include <thread>
#include <chrono>

#include "Types.h"
#include "CallbackManager.h"
#include "TSock.h"

#include "TestDefine.h"

GameService* g_service = nullptr;

void reqTest(NLNET::TSockPtr sock, NLNET::CMessage& msg)
{
    auto s = msg.getData();
    std::cout << "req:" << msg.m_msg_name << " content:" << s << "\n";
    s += " req_user_data";

    NLNET::CMessage req_db{def::_REQ_USER_DATA, s};
    g_service->m_network->send(def::DBS_SID, req_db);
}

void rspUserData(NLNET::TSockPtr sock, NLNET::CMessage& msg)
{
    auto s = msg.getData();
    std::cout << "receive:" << msg.m_msg_name << " content:" << s << "\n";
    s += " GS-rspUserData";

    NLNET::CMessage rsp{def::_RSP_TEST, s};
    sock->sendMsg(rsp);
}

NLNET::MsgCallbackArray call_array = 
{
    {def::_REQ_TEST, &reqTest},
    {def::_RSP_USER_DATA, &rspUserData},
};

GameService::GameService()
    : m_network()
{
}

GameService::~GameService()
{
    if (m_network) {
        m_network->stop();
        m_network->waitThreadExit();
    }
}

bool GameService::start()
{
    m_network = std::make_unique<NLNET::UnifiedNetwork>();
    if (!m_network->init(def::GS_SID, def::GS_NAME, def::GS_ADDR)) {
        std::cout << "network init faild!\n";
        return false;  
    }

    m_network->addService(def::DBS_SID, def::DBS_NAME, def::DBS_ADDR, false);

    m_network->getCallbackManager().setMsgCallbackArray(std::move(call_array));
    return true;
}

void GameService::update(uint32_t diff)
{
    m_network->update(diff);
} 

int main()
{
    GameService gs{};
    g_service = &gs;
    if (!gs.start())
        return 0;

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    int n = 0;
    while (true) {
        auto tnow = std::chrono::system_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(tnow - tp).count();
        gs.update((uint32_t)delta);
        tp = tnow;
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
        ++n;
    }
    return 0;
}
