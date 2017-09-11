#include "RedisCpp.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstring>

#include "TestTool.h"

std::string ip = "127.0.0.1";
int port = 6379;

rediscpp::Connection g_context;

struct Timer
{
    Timer()
    {
        m_start = std::chrono::system_clock::now();
        m_end = m_start;
    }

    ~Timer()
    {
    }

    void end()
    {
        m_end = std::chrono::system_clock::now();
    }

    std::pair<uint64_t, uint64_t> cost() const
    {
        auto val = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
        return { val / 1000, val%1000};
    }

    std::chrono::system_clock::time_point m_start;
    std::chrono::system_clock::time_point m_end;
};

bool testPerformance()
{
    try {
        rediscpp::Buffer key{"test_hash"};
        rediscpp::DEL(g_context, key);

        rediscpp::Hash redis{ g_context };
        std::vector<uint8_t> big_data{};
        big_data.resize(1024, 'b');
        //int n = 1000000;
        int n = 1000 * 1000 * 10;
        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                redis.HSET(key, rediscpp::Buffer{i}, rediscpp::Buffer{ big_data });
            }
            tm.end();

            auto cst = tm.cost();
            printf("cost:%lu.%03lu\n", cst.first, cst.second);
        }
        return true;
    } catch (const rediscpp::Exception& e) {
        std::cout << "RedisException:" << __LINE__ << ":" << __FUNCTION__ << ":" << e.what() << "\n";
        return false;
    }
    return true;
}

int main()
{
    auto context = rediscpp::redisConnect(ip, port);
    if (!context) {
        std::cout << "error context error\n";
        return 0;
    }
    g_context = std::move(context);

    testPerformance();
    return 0;
}