#include "GameService.h"

#include <thread>
#include <chrono>

#include "Types.h"
#include "CallbackManager.h"
#include "TSock.h"

const NLNET::ServiceID S_ID = 12;
const std::string S_NAME = "GS";
const NLNET::CInetAddress S_ADDR = {"127.0.0.1", 22001};

const std::string _REQ_TEST = "_REQ_TEST";
const std::string _RSP_TEST = "_RSP_TEST";

void reqTest(NLNET::TSockPtr sock, NLNET::CMessage& req)
{
    auto s = req.getData();
    std::cout << "req:" << req.m_msg_name << " content:" << s << "\n";
    s += "_rsp";

    NLNET::CMessage rsp{_RSP_TEST, s};
    sock->sendMsg(rsp);
}

NLNET::MsgCallbackArray call_array = 
{
    {_REQ_TEST, &reqTest},
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
    if (!m_network->init(S_ID, S_NAME, S_ADDR)) {
        std::cout << "network init faild!\n";
        return false;  
    }

    m_network->getCallbackManager().setMsgCallbackArray(std::move(call_array));
    return true;
}

bool GameService::sendMsg(std::string s)
{
    (void)s;
    return true;
    /*
    NLNET::CMessage msg{_REQ_TEST, s};
    return m_network->send(S_ID, msg);
    */
}

void GameService::update(uint32_t diff)
{
    m_network->update(diff);
} 

int main()
{
    GameService gs{};
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
