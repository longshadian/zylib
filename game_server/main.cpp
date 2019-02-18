#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

#include "ServerApp.h"

struct SleepForLog
{
    SleepForLog(int seconds)
        : m_seconds(seconds)
    {
    }

    ~SleepForLog()
    {
        if (m_seconds > 0)
            std::this_thread::sleep_for(std::chrono::seconds(m_seconds));
    }
    int m_seconds;
};

int main(int argc, char** argv)
{
    SleepForLog for_log{2};
    (void)for_log;
    if (argc == 1) {
        if (!ServerApp::instance().start()) {
            return EXIT_FAILURE;
        }
        ServerApp::instance().loop();
    } else {
    }
    return EXIT_SUCCESS;
}