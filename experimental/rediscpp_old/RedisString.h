#pragma once

#include "RedisType.h"

namespace rediscpp {

class RedisString
{
public:
    RedisString(RedisContextGuard& context);
    ~RedisString() = default;

    void SET(Buffer key, Buffer value);
    Buffer GET(Buffer key);

    long long INCR(Buffer key);
    long long INCRBY(Buffer key, long long increment);

    std::vector<Buffer> KEYS(Buffer key);
private:
    RedisContextGuard& m_context;
};

}