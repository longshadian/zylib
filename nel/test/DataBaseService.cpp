#include "DataBaseService.h"

#include <thread>
#include <chrono>

#include "Types.h"
#include "CallbackManager.h"
#include "TSock.h"

#include "TestDefine.h"

void reqUserData(NLNET::TSockContext& sock, NLNET::CMessage& req)
{
    auto s = req.getData();
    std::cout << "req:" << req.m_msg_name << " content:" << s << "\n";
    s += " rsp from dbs  user_data:xxxx";
    NLNET::CMessage rsp{ def::_RSP_USER_DATA, s};
    sock.m_sock->sendMsg(rsp);
}

DataBaseService::DataBaseService()
    : m_network()
{
}

DataBaseService::~DataBaseService()
{
    if (m_network) {
        m_network->stop();
        m_network->waitThreadExit();
    }
}

bool DataBaseService::start()
{
    m_network = std::make_unique<NLNET::UnifiedNetwork>();
    if (!m_network->init(def::DBS_SID, def::DBS_NAME, def::DBS_ADDR)) {
        std::cout << "network init faild!\n";
        return false;  
    }

    NLNET::MsgCallbackArray call_array = 
    {
        {def::_REQ_USER_DATA, &reqUserData },
    };

    m_network->getCallbackManager().setMsgCallbackArray(std::move(call_array));
    return true;
}

bool DataBaseService::sendMsg(std::string s)
{
    (void)s;
    return true;
}

void DataBaseService::update(uint32_t diff)
{
    m_network->update(diff);
} 

int main()
{
    DataBaseService gs{};
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
