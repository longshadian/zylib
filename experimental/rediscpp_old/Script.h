#pragma once

#include "RedisType.h"

namespace rediscpp {

class Script
{
public:
    Script(RedisContextGuard& context);
    ~Script() = default;

    Buffer LOAD(Buffer cmd);

    //¿ÉÄÜ·µ»Øarray
    Buffer EVAL(Buffer cmd, std::vector<Buffer> keys = {}, std::vector<Buffer> values = {});
    Buffer EVALSHA(Buffer cmd, std::vector<Buffer> keys = {}, std::vector<Buffer> values = {});
private:
    Buffer evalInternal(std::string eval_cmd, Buffer cmd, std::vector<Buffer> keys, std::vector<Buffer> values);
    static Buffer luaToRedis(const redisReply* reply);
private:
    RedisContextGuard& m_context;
};

}