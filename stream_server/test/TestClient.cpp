#include <iostream>
#include <thread>
#include <atomic>
#include <memory>

#include "TestTool.h"
#include "zylib/zylib.h"
#include "stream_server.h"

void rsp_ping(network::CallbackMessageContext& context, network::CallbackMessagePtr msg)
{
    (void)context;
    LOG(DEBUG) << "received " << getString(*msg);
}


int main()
{
    zylib::logger::initSyncConsole();

    bool conn_succ = false;
    boost::asio::io_service io_service{};
    network::CallbackClient c{io_service};
    c.setCB_SyncConnectSuccess([&conn_succ](network::CallbackClient&)
        {
            conn_succ = true;
            LOG(DEBUG) << "conn success";
        });

    c.setCB_SyncConnectFail([&conn_succ](network::CallbackClient&)
        {
            LOG(DEBUG) << "conn fail";
        });

    network::CallbackServer::CB_SyncReceivedMsgArray msg_array = 
    {
        {2, std::bind(&rsp_ping, std::placeholders::_1, std::placeholders::_2)}
    };
    c.setCB_SyncReceivedMessage(std::move(msg_array));

    c.connect("127.0.0.1", 32222);

    zylib::TimingWheel w{ std::chrono::seconds{3} };

    std::thread t([&io_service]()
    {
        io_service.run();
    });

    while (true) {
        auto delta = std::chrono::milliseconds{ 50 };
        std::this_thread::sleep_for(delta);
        c.update(delta);

        if (conn_succ) {
            w.update(delta);
            if (w.passed()) {
                w.reset();

                auto tnow = std::time(nullptr);
                auto msg = createMessage(1, std::to_string(tnow));
                c.sendMessage(msg);
            }
        }
    }

    t.join();
    return 0;
}
