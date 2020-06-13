
#include <chrono>
#include <thread>

#include <zysoft/timer/timing_wheel.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"

static void Test_TimingWheel1()
{
    zysoft::timing_wheel<int, 10> tw;

    tw.add(30, 3);
    tw.add(31, 3);
    tw.add(32, 3);
    tw.add(39, 3);

    tw.add(10, 1);
    tw.add(11, 1);
    tw.add(12, 1);
    tw.add(13, 1);

    tw.add(66, 6);
    tw.add(65, 6);

    tw.add(107, 10);

    tw.add(1300, 13);
    tw.add(1311, 13);
    tw.add(1302, 13);
    tw.add(1302, 13);
    tw.add(1311, 13);

    tw.add(1600, 16);

    int n = 0;
    while (1) {
        ++n;
        tw.tick();
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        auto vec = tw.get_expired();
        LogInfo << "n: " << n << " expired count: " << vec.size();
        for (const auto& p : vec) {
            LogInfo << "\t" << n << "\t value: " << p.first << "\t duration: " << p.second;
        }
        LogInfo << "\n";
        if (n >= 20)
            break;
    }
}

static void Test_TimingWheel2()
{
    zysoft::timing_wheel<std::unique_ptr<int>, 10> tw;

    tw.add(std::make_unique<int>(30), 3);
    tw.add(std::make_unique<int>(31), 3);
    tw.add(std::make_unique<int>(32), 3);
    tw.add(std::make_unique<int>(39), 3);

    tw.add(std::make_unique<int>(10), 1);
    tw.add(std::make_unique<int>(11), 1);
    tw.add(std::make_unique<int>(12), 1);
    tw.add(std::make_unique<int>(13), 1);

    auto hdl = tw.add(std::make_unique<int>(66), 6);
    tw.add(std::make_unique<int>(65), 6);

    tw.add(std::make_unique<int>(107), 10);

    tw.add(std::make_unique<int>(1300), 13);
    tw.add(std::make_unique<int>(1311), 13);
    tw.add(std::make_unique<int>(1302), 13);
    tw.add(std::make_unique<int>(1302), 13);
    tw.add(std::make_unique<int>(1311), 13);

    tw.add(std::make_unique<int>(1600), 16);

    int n = 0;
    while (1) {
        ++n;
        tw.tick();
        if (n == 3) {
            tw.reset(hdl);
        }
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        auto vec = tw.get_expired();
        LogInfo << "n: " << n << " expired count: " << vec.size();
        for (const auto& p : vec) {
            LogInfo << "\t" << n << "\t value: " << *p.first << "\t duration: " << p.second;
        }
        LogInfo << "\n";
        if (n >= 20)
            break;
    }
}

#if 1
TEST_CASE("test timing wheel")
{
    PrintInfo("test timing wheel");
    try {
        Test_TimingWheel2();
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        CHECK(false);
    }
}
#endif 

