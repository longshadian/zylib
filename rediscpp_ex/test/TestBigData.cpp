#include "RedisCpp.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstring>

#include "TestTool.h"
#include "bigdata.pb.h"

std::string ip = "127.0.0.1";
int port = 6379;

using namespace rediscpp;

Connection g_context;

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

    uint64_t cost() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
    }

    std::chrono::system_clock::time_point m_start;
    std::chrono::system_clock::time_point m_end;
};


struct BigData
{
    BigData()
        : m_data()
    {
        m_data.resize(20 * 16 * 50);
    }

    std::vector<uint8_t> getBinary() const
    {
        std::vector<uint8_t> buffer{};
        buffer.resize(m_data.size() * sizeof(uint64_t));
        std::memcpy(buffer.data(), m_data.data(), buffer.size());
        return buffer;
    }

    std::vector<uint64_t> m_data;
};

bool testBigData()
{
    try {
        Buffer key{ "a" };
        DEL(g_context, key);

        BigData bd{};
        String redis{ g_context };

        int n = 10000;
        {
            auto temp = bd.getBinary();
            std::cout << "size:" << temp.size() << "\n";
        }

        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                auto val = bd.getBinary();
                redis.SET(key, Buffer{val});
            }
            tm.end();

            std::cout << "cost:" << tm.cost() << "\n";
        }

        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                auto val = bd.getBinary();
                redis.SET(key, Buffer{std::move(val)});
            }
            tm.end();
            std::cout << "cost:" << tm.cost() << "\n";
        }

        return true;
    } catch (const Exception& e) {
        std::cout << "RedisException:" << __LINE__ << ":" << __FUNCTION__ << ":" << e.what() << "\n";
        return false;
    }
    return true;
}

std::vector<uint8_t> getProto(const obj_big_data& bg)
{
    std::vector<uint8_t> buffer{};
    buffer.resize(bg.ByteSize());
    bg.SerializeToArray(buffer.data(), buffer.size());
    return buffer;
}

bool testProto()
{
    try {
        Buffer key{ "a" };
        DEL(g_context, key);

        String redis{ g_context };

        int n = 10000;

        obj_big_data bg{};
        for (int i = 0; i != n; ++i) {
            bg.add_records(i);
        }

        std::cout << "size:" << bg.ByteSize() << "\n";
        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                auto val = getProto(bg);
                redis.SET(key, Buffer{val});
            }
            tm.end();
            std::cout << "cost:" << tm.cost() << "\n";
        }

        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                auto val = getProto(bg);
                redis.SET(key, Buffer{std::move(val)});
            }
            tm.end();
            std::cout << "cost:" << tm.cost() << "\n";
        }

        return true;
    } catch (const Exception& e) {
        std::cout << "RedisException:" << __LINE__ << ":" << __FUNCTION__ << ":" << e.what() << "\n";
        return false;
    }
    return true;
}

bool testRedisLua()
{
    std::string STR =
        " userid_str = \"1:2:3:4:5:6:7:8\" "
        " local userid_list = {} "
        " for w in string.gmatch(userid_str, \"%d+\") do "
        "       table.insert(userid_list, w) "
        " end "
        " for i = 1, #userid_list do "
        "   print(userid_list[i]) "
        " end ";
    try {
        Buffer key{ "a" };
        DEL(g_context, key);

        String redis{ g_context };

        int n = 10000;

        obj_big_data bg{};
        for (int i = 0; i != n; ++i) {
            bg.add_records(i);
        }

        std::cout << "size:" << bg.ByteSize() << "\n";
        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                auto val = getProto(bg);
                redis.SET(key, Buffer{ val });
            }
            tm.end();
            std::cout << "cost:" << tm.cost() << "\n";
        }

        {
            Timer tm{};
            for (int i = 0; i != n; ++i) {
                auto val = getProto(bg);
                redis.SET(key, Buffer{ std::move(val) });
            }
            tm.end();
            std::cout << "cost:" << tm.cost() << "\n";
        }

        return true;
    }
    catch (const Exception& e) {
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

    testBigData();
    std::cout << "========\n";
    testProto();
    return 0;
}