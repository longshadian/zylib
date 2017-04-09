#pragma once

#include <vector>
#include <string>

#include "Exception.h"
#include "Buffer.h"

class redisReply;

namespace rediscpp { namespace detail {
template <typename T>
struct TypeToBuffer
{
    static Buffer toBuffer(T&& t)
    {
        return Buffer(std::forward<T>(t));
    }
};

inline
void mergeKeys(int key_num, std::vector<Buffer>* keys, std::vector<Buffer>* values)
{
    (void)key_num;
    (void)keys;
    (void)values;
}

template <typename T, typename... Args>
inline
void mergeKeys(int key_num, std::vector<Buffer>* keys, std::vector<Buffer>* values, T&& t, Args&&... args)
{
    if (static_cast<int>(keys->size()) < key_num) {
        keys->emplace_back(std::move(TypeToBuffer<decltype(t)>::toBuffer(std::forward<T>(t))));
    } else {
        values->emplace_back(std::move(TypeToBuffer<decltype(t)>::toBuffer(std::forward<T>(t))));
    }
    mergeKeys(key_num, keys, values, std::forward<Args>(args)...);
}

}
}

//////////////////////////////////////////////////////////////////////////

namespace rediscpp {

class Connection;
class Buffer;
class BufferArray;

class Script
{
public:
    Script(Connection& conn);
    ~Script() = default;

    Buffer LOAD(Buffer cmd);

    BufferArray EVAL(Buffer cmd, std::vector<Buffer> keys = {}, std::vector<Buffer> values = {});
    BufferArray EVALSHA(Buffer cmd, std::vector<Buffer> keys = {}, std::vector<Buffer> values = {});

    template <typename... Args>
    BufferArray EVALSHA(Buffer cmd, int key_num, Args&&... args)
    {
        if (size_t(key_num) > sizeof...(args))
            throw Exception("EVALSHA too much key num");

        std::vector<Buffer> keys;
        std::vector<Buffer> values;
        detail::mergeKeys(key_num, &keys, &values, std::forward<Args>(args)...);
        return evalInternal("EVALSHA", std::move(cmd), std::move(keys), std::move(values));
    }
private:
    BufferArray evalInternal(std::string eval_cmd, Buffer cmd, std::vector<Buffer> keys, std::vector<Buffer> values);

    static BufferArray luaToRedis(const redisReply* reply);
private:
    Connection& m_conn;
};

}
