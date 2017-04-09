#pragma once


namespace rediscpp {

class Buffer;
class Connection;

class Set
{
public:
    Set(Connection& context);
    ~Set() = default;

    long long SADD(Buffer key, Buffer value);
    bool SISMEMBER(Buffer key, Buffer value);
private:
    Connection& m_context;
};

}