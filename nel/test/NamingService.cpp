#include "NamingService.h"

#include <ctime>
#include <iostream>
#include <string>
#include <chrono>

#include "unified_network.h"
#include "unified_connection.h"
#include "NetServer.h"

NamingService::NamingService()
{
}

NamingService::~NamingService()
{
}

void NamingService::start()
{
    NLNET::UnifiedNetwork network;
    if (!network.init("ns", NLNET::CInetAddress{"127.0.0.1", 22001})) {
        std::cout << "init error\n";
        return;
    }
}

void test()
{
        NamingService ns{};
        ns.start();
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
}

int main()
{
    boost::asio::io_service io_service{};
    boost::asio::io_service::work work{io_service};

    NLNET::UnifiedConnection c{};
    try {
        auto s = std::make_shared<NLNET::NetServer>(io_service, "192.0.0.1", 22001, c);
        s->accept();

        io_service.run();
    } catch (const std::exception& e) {
        std::cout << "main exception:" << e.what() << "\n";
    }

    std::cout << "out\n";
    return 0;
}
