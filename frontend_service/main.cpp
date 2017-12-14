#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

#include <iostream>

#include "Service.h"

#include "FakeLog.h"

/*
int testFun()
{
    ::pt::json_wechat_login rsp{};
    rsp.add_privilege("a");
    rsp.add_privilege("b");
    rsp.add_privilege("呵呵");
    rsp.state();
    std::string s;
    if (google::protobuf::util::MessageToJsonString(rsp, &s).ok()) {
        std::cout << s << "\n";
    } else {
        std::cout << "error\n";
    }
    return 0;
}
*/

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
        if (!Service::instance().start()) {
            return EXIT_FAILURE;
        }
        Service::instance().loop();
    } else {
        ServerCmd cmd{};
        cmd.exec(argc, argv);
    }
    LOG(INFO) <<  "main EXIT_SUCCESS";
    return EXIT_SUCCESS;
}