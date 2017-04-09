#pragma once

#include <vector>
#include <string>

class redisReply;

namespace rediscpp {

class Connection;
class Buffer;
class BufferArray;

class Script
{
public:
    Script(Connection& context);
    ~Script() = default;

    Buffer LOAD(Buffer cmd);

    BufferArray EVAL(Buffer cmd, std::vector<Buffer> keys = {}, std::vector<Buffer> values = {});
    BufferArray EVALSHA(Buffer cmd, std::vector<Buffer> keys = {}, std::vector<Buffer> values = {});
private:
    BufferArray evalInternal(std::string eval_cmd, Buffer cmd, std::vector<Buffer> keys, std::vector<Buffer> values);
    static BufferArray luaToRedis(const redisReply* reply);
private:
    Connection& m_context;
};

}
