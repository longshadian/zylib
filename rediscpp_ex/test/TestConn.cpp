#include "RedisCpp.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "TestTool.h"

std::string ip = "127.0.0.1";
int port = 6379;

using namespace rediscpp;

Connection g_context;

bool test()
{
    Buffer key{ "a" };
    String redis{ g_context };
    try {
        std::cout << redis.GET(key).asInt() << "\n";

        int n = 10;
        while (--n) {
            std::cout << n << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << redis.GET(key).asInt() << "\n";
        return true;
    } catch (const ConnectionException& e) {
        std::cout << "rediscpp ConnectionException:" << e.what() 
            << " context_err:" << g_context.getRedisContext()->err 
            << " " << g_context.getRedisContext()->errstr << "\n";
        if (g_context.reconnection()) {
            std::cout << "reconnection success\n";
            std::cout << redis.GET(key).asInt() << "\n";
        } else {
            std::cout << "reconnection fail\n";
        }
    } catch (const ReplyException& e) {
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

    test();
    return 0;
}