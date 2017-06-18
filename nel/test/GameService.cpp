#include "GameService.h"

#include <thread>
#include <chrono>

#include "Types.h"
#include "CallbackManager.h"
#include "TSock.h"
#include "TestDefine.h"

#include "GSCallbackManager.h"
GameService* g_service = nullptr;

void reqTest(NLNET::TSockContext sock, NLNET::CMessage& msg)
{
    auto s = msg.getData();
    std::cout << "req:" << msg.m_msg_name << " content:" << s << "\n";
    s += " req_user_data";

    NLNET::CMessage req_db{def::_REQ_USER_DATA, s};
    g_service->m_network->send(def::DBS_SID, req_db, def::DBS_ADDR);
    g_service->getCBMgr().regCBRspUserData(12,
        [sock_hdl = sock.m_sock->getSockHdl()](NLNET::CMessage& rmsg)
        {
            auto rsock = sock_hdl.lock();
            if (!rsock) {
                std::cout << "rsock is disconnection!\n";
                return;
            }

            auto str = rmsg.getData();
            std::cout << "receive:" << rmsg.m_msg_name << " content:" << str << "\n";
            str += " GS-rspUserData";
            NLNET::CMessage rsp{def::_RSP_TEST, str};
            rsock->sendMsg(rsp);
        });
}

void rspUserData(NLNET::TSockContext sock, NLNET::CMessage& msg)
{
    (void)sock;
    g_service->getCBMgr().callback(12, msg);
}

GameService::GameService()
    : m_network()
    , m_gs_cb_mgr()
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
    m_gs_cb_mgr = std::make_unique<GSCallbackManager>();

    NLNET::MsgCallbackArray call_array = 
    {
        {def::_REQ_TEST, &reqTest},
        {def::_RSP_USER_DATA, &rspUserData},
    };
    m_network->getCallbackManager().setMsgCallbackArray(std::move(call_array));
    m_network->getCallbackManager().setServiceUpCallback(def::DBS_NAME,
        std::bind(&GameService::cbServiceUp, this, std::placeholders::_1));
    m_network->getCallbackManager().setServiceDownCallback(def::DBS_NAME,
        std::bind(&GameService::cbServiceDown, this, std::placeholders::_1));

    if (!m_network->init(def::GS_SID, def::GS_NAME, def::GS_ADDR)) {
        std::cout << "network init faild!\n";
        return false;  
    }

    m_network->addService(def::DBS_SID, def::DBS_NAME, def::DBS_ADDR, true);

    return true;
}

void GameService::update(uint32_t diff)
{
    m_network->update(diff);
} 

GSCallbackManager& GameService::getCBMgr()
{
    return *m_gs_cb_mgr;
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
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
        ++n;
    }
    return 0;
}

void GameService::cbServiceUp(NLNET::TSockContext& sock)
{
    std::cout << "sock up. sid:" << sock.m_service_id
        << " sname:" << sock.m_service_name
        << "\n";
}

void GameService::cbServiceDown(NLNET::TSockContext& sock)
{
    std::cout << "sock down. sid:" << sock.m_service_id
        << " sname:" << sock.m_service_name
        << "\n";
}
