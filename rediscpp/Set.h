#pragma once

#include <cstdint>
#include <vector>

namespace rediscpp {

class Buffer;
class Connection;

class Set
{
public:
    Set(Connection& conn);
    ~Set() = default;

    long long SADD(Buffer key, Buffer value);
    bool SISMEMBER(Buffer key, Buffer value);
    long long SCARD(Buffer key);
    std::vector<Buffer> SRANDMEMBER(Buffer key, int32_t len);
private:
    Connection& m_conn;
};

}