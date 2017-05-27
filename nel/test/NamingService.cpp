#include "NamingService.h"

#include <ctime>
#include <iostream>
#include <string>
#include <chrono>

#include "unified_network.h"

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

int main()
{
    NamingService ns{};
    ns.start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
