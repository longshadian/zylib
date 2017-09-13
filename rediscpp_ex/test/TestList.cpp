#include "rediscpp.h"

#include <cstdio>
#include <string>
#include <chrono>
#include <iostream>
#include "TestTool.h"

std::string ip = "127.0.0.1";
int port = 6379;

using namespace rediscpp;

Connection g_context;

bool test()
{
    try {
        Buffer key{"a"};
        DEL(g_context, key);

        List redis{g_context};
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

bool testLTRM()
{
    try {
        Buffer key{"a"};
        DEL(g_context, key);

        List redis{g_context};
        TEST(redis.RPUSH(key, Buffer("a")) == 1);
        TEST(redis.RPUSH(key, Buffer("b")) == 2);
        TEST(redis.RPUSH(key, Buffer("c")) == 3);
        TEST(redis.RPUSH(key, Buffer("d")) == 4);
        TEST(redis.RPUSH(key, Buffer("e")) == 5);
        TEST(redis.RPUSH(key, Buffer("f")) == 6);
        TEST(redis.LLEN(key) == 6);

        auto ret1 = redis.LRANGE(key, 0, -1);
        pout(ret1);

        redis.LTRIM(key, 2, -1);
        auto ret2 = redis.LRANGE(key, 0, -1);
        pout(ret2);

        redis.LTRIM(key, 2, -1);
        auto ret3 = redis.LRANGE(key, 0, -1);
        pout(ret3);

        redis.LTRIM(key, 0, -1);
        auto ret4 = redis.LRANGE(key, 0, -1);
        pout(ret4);

        redis.LTRIM(key, -1, 0);
        auto ret5 = redis.LRANGE(key, 0, -1);
        pout(ret5);

        return true;
    } catch (const Exception& e) {
        std::cout << "RedisException:" << __LINE__ << ":" << __FUNCTION__ << ":" << e.what() << "\n";
        return false;
    }
}

bool testLINDEX()
{
    try {
        Buffer key{"a"};
        DEL(g_context, key);

        List redis{g_context};
        TEST(redis.RPUSH(key, Buffer("a")) == 1);
        TEST(redis.RPUSH(key, Buffer("b")) == 2);
        TEST(redis.RPUSH(key, Buffer("c")) == 3);
        TEST(redis.RPUSH(key, Buffer("d")) == 4);
        TEST(redis.RPUSH(key, Buffer("e")) == 5);
        TEST(redis.RPUSH(key, Buffer("f")) == 6);

        auto b = redis.LINDEX(key, 0);
        pout(b);

        b = redis.LINDEX(key, 1);
        pout(b);

        b = redis.LINDEX(key, 2);
        pout(b);

        b = redis.LINDEX(key, 20);
        pout(b);
        std::cout << b.empty() << "\n";

        return true;
    } catch (const Exception& e) {
        std::cout << "RedisException:" << __LINE__ << ":" << __FUNCTION__ << ":" << e.what() << "\n";
        return false;
    }
}


int main()
{
    auto context = rediscpp::redisConnect(ip, port);
    if (!context) {
        std::cout << "error context error\n";
        return 0;
    }
    g_context = std::move(context);

    //test();
    //testLTRM();
    testLINDEX();
    return 0;
}
