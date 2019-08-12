#include <iostream>
#include <thread>
#include <atomic>
#include <memory>

#include "stream_server.h"
#include "zylib.h"

#include "TestTool.h"

void req_ping(network::CallbackMessageContext& context, network::CallbackMessagePtr msg)
{
    LOG(DEBUG) << getString(*msg);

    auto s = getString(*msg);
    s += "_xxx";
    auto conn = context.m_hdl.lock();
    if (conn) {
        conn->sendMessage(createMessage(2, s));
    }
}

int main()
{
    zylib::logger::initSyncConsole();

    network::CallbackServer s{"127.0.0.1", 32222};
    network::CallbackServer::CB_SyncReceivedMsgArray msg_array = 
    {
        {1, std::bind(&req_ping, std::placeholders::_1, std::placeholders::_2)}
    };
    s.setCB_SyncReceivedMessage(std::move(msg_array));

    s.start();

    while (true) {
        auto delta = std::chrono::milliseconds{10};
        std::this_thread::sleep_for(delta);
        s.update(delta);
    }
    return 0;
}
