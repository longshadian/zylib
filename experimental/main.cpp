#include "FakeLog.h"

#include <iostream>
#include <thread>
#include <chrono>

class LogConsole : public fake_log::LogStream
{
public:
    virtual void flush(std::string str) override
    {
        std::cout << str << std::endl;
    }
};

void fun()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds{1});
        auto id = std::this_thread::get_id();
        FAKE_LOG(DEBUG)     << "DEBUG   " << id;
        FAKE_LOG(INFO)      << "INFO    " << id;
        FAKE_LOG(WARNING)   << "WARNING " << id;
        FAKE_LOG(ERROR)     << "ERROR   " << id;
    }
}

int main()
{
    fake_log::initLog(std::make_unique<LogConsole>(), fake_log::INFO);

    std::thread t{std::bind(&fun)};

    fun();

    t.join();

    return 0;
}
