#include "rediscpp.h"

#include <cstdio>
#include <string>
#include <chrono>
#include <iostream>
#include <thread>

#include "TestTool.h"

std::string ip = "127.0.0.1";
int port = 6379;

using namespace rediscpp;

std::shared_ptr<ConnectionPool> g_pool = nullptr;

bool test()
{
    try {
        ConnectionGuard guard{*g_pool};
        if (!guard) {
            std::cout << "conn empty\n";
            return false;
        } else {
            std::cout << "conn success " << guard->getRedisContext() << "\n";
        }

        Buffer key{"a"};
        DEL(*guard, key);

        List redis{*guard};
        TEST(redis.RPUSH(key, Buffer("b")) == 1);
        TEST(redis.RPUSH(key, Buffer(123)) == 2);
        TEST(redis.LPUSH(key, Buffer(12.98)) == 3);
        TEST(redis.LLEN(key) == 3);

        auto ret1 = redis.LRANGE(key, 0, -1);
        pout(ret1);

        auto ret2 = redis.RPOP(key);
        pout(ret2);

        auto ret3 = redis.LRANGE(key, 0, -1);
        pout(ret3);

        auto ret4 = redis.LPOP(key);
        pout(ret4);

        auto ret5 = redis.LRANGE(key, 0, -1);
        pout(ret5);

        return true;
    } catch (const Exception& e) {
        std::cout << "RedisException:" << __LINE__ << ":" << __FUNCTION__ << ":" << e.what() << "\n";
        return false;
    }
}

int main()
{
    rediscpp::ConnectionOpt opt{};
    opt.m_ip = ip;
    opt.m_port = port;
    g_pool = std::make_shared<ConnectionPool>(std::move(opt));
    if (!g_pool->init()) {
        std::cout << "pool init failed\n";
        return false;
    }

    int n = 10;
    while (n > 0) {
        --n;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "n:" << n << "\n";
        test();
    }

    std::cout << "test success\n";
    return 0;
}
