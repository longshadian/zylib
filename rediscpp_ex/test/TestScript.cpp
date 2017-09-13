#include "rediscpp.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "TestTool.h"

std::string ip = "127.0.0.1";
int port = 6379;

using namespace rediscpp;

Connection g_context;

bool test_LOAD()
{
    try {
        auto file_content = catFile("./test_evalsha.lua");
        Script redis{g_context};
        auto id = redis.LOAD(Buffer(file_content));
        auto id_str = id.asString();
        std::cout << "id:" << id_str << "\n";

        Reply reply{reinterpret_cast<redisReply*>(::redisCommand(g_context.getRedisContext(), "evalsha %s %d", id_str.c_str(), 0))};
        redisReply* r = reply.getRedisReply();
        std::cout << "type:   " << r->type << "\n";
        std::cout << "integer:" << r->integer << "\n";
        std::cout << "len:    " << r->len << "\n";
        std::cout << "elements:" << r->elements << "\n";

        if (r->elements == 3) {
            std::cout << "----------\n"; 
            std::cout << r->element[0]->str << "\n";
            std::cout << r->element[1]->integer << "\n";
            std::cout << r->element[2]->type << "\n";
        }

        std::cout << "----------\n"; 
        auto arr = redis.EVALSHA(id);
        pout(arr);
        std::cout << "\n";
        return true;
    } catch (const Exception& e) {
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

    test_LOAD();
    return 0;
}
