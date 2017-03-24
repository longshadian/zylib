#pragma once

#include <hiredis.h>

#include <string>
#include <vector>
#include <memory>

namespace rediscpp {

class Buffer
{
public:
    enum TYPE
    {
        BUFFER = 1,
        ARRAY  = 2,
    };
public:
    Buffer();
    ~Buffer() = default;

    Buffer(TYPE t);
    Buffer(const std::string& str);
    Buffer(const char* p, int len);
    Buffer(std::vector<uint8_t> data);

    Buffer(const Buffer& rhs);
    Buffer& operator=(const Buffer& rhs);

    Buffer(Buffer&& rhs);
    Buffer& operator=(Buffer&& rhs);

    TYPE getType() const;

    /************************************************************************
     * buffer²Ù×÷                                                                 
     ************************************************************************/
    std::string asString() const;

    int asInt() const;
    int8_t asInt8() const;
    int16_t asInt16() const;
    int32_t asInt32() const;
    int64_t asInt64() const;

    uint8_t asUInt8() const;
    uint16_t asUInt16() const;
    uint32_t asUInt32() const;
    uint64_t asUInt64() const;

    double asDouble() const;
    float asFloat() const;

    const uint8_t* getData() const;
    size_t getLen() const;
    const std::vector<uint8_t>& getDataVector() const;
    bool empty() const;
    bool isBuffer() const;
    void clearBuffer();

    void append(std::string s);
    void append(const void* data, size_t len);



    /************************************************************************
     * array²Ù×÷                                                                 
     ************************************************************************/
    bool isArray() const;
    void clearArray();
    const Buffer& operator[](size_t index) const;
    Buffer& operator[](size_t index);
    void pushBack(Buffer&& blk);
    size_t arraySize() const;

private:
    template <typename T>
    T asIntDetail() const
    {
        static_assert(std::is_integral<T>::value, "T must be Integeral!");
        auto s = asString();
        if (s.empty())
            return T();
        return static_cast<T>(std::atoll(s.c_str()));
    }
private:
    TYPE                    m_type;
    std::vector<uint8_t>    m_data;
    std::vector<Buffer> m_array;
};

struct FreeReplyObject
{
    void operator()(redisReply* x)
    {
        if (x)
            freeReplyObject(x);
    }
};

struct RedisFreeObject
{
    void operator()(redisContext* x)
    {
        if (x)
            redisFree(x);
    }
};

typedef std::unique_ptr<redisContext, RedisFreeObject> RedisContextGuard;

typedef std::unique_ptr<redisReply, FreeReplyObject> RedisReplyGuard;

}