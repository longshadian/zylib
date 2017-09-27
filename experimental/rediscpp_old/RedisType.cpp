#include "RedisType.h"

namespace rediscpp {

Buffer::Buffer()
    : m_type(BUFFER)
    , m_data()
    , m_array()
{
}

Buffer::Buffer(TYPE t)
    : m_type(t)
    , m_data()
    , m_array()
{
}

Buffer::Buffer(const std::string& str)
{
    m_type = BUFFER;
    if (str.empty()) {
        return;
    }
    auto ptr = reinterpret_cast<const uint8_t*>(str.data());
    m_data.assign(ptr, ptr + str.size());
}

Buffer::Buffer(const char* p, int len)
{
    m_type = BUFFER;
    if (len <= 0)
        return;
    auto ptr = reinterpret_cast<const uint8_t*>(p);
    m_data.assign(ptr, ptr + len);
}

Buffer::Buffer(std::vector<uint8_t> data)
    : m_type(BUFFER)
    , m_data(std::move(data))
    , m_array()
{
}

Buffer::Buffer(const Buffer& rhs)
    : m_type(rhs.m_type)
    , m_data(rhs.m_data)
    , m_array(rhs.m_array)
{
}

Buffer& Buffer::operator=(const Buffer& rhs)
{
    if (this != &rhs) {
        m_type = rhs.m_type;
        m_data = rhs.m_data;
        m_array = rhs.m_array;
    }
    return *this;
}

Buffer::Buffer(Buffer&& rhs)
    : m_type(rhs.m_type)
    , m_data(std::move(rhs.m_data))
    , m_array(std::move(rhs.m_array))
{
}

Buffer& Buffer::operator=(Buffer&& rhs)
{
    if (this != &rhs) {
        m_type = rhs.m_type;
        m_data = std::move(rhs.m_data);
        m_array = std::move(rhs.m_array);
    }
    return *this;
}

std::string Buffer::asString() const
{
    if (m_data.empty())
        return {};
    auto p = reinterpret_cast<const char*>(m_data.data());
    return std::string(p,  p + m_data.size());
}

int Buffer::asInt() const
{
    return asIntDetail<int>();
}

int8_t Buffer::asInt8() const
{
    return asIntDetail<int8_t>();
}

int16_t Buffer::asInt16() const
{
    return asIntDetail<int16_t>();
}

int32_t Buffer::asInt32() const
{
    return asIntDetail<int32_t>();
}

int64_t Buffer::asInt64() const
{
    return asIntDetail<int64_t>();
}

uint8_t Buffer::asUInt8() const
{
    return asIntDetail<uint8_t>();
}

uint16_t Buffer::asUInt16() const
{
    return asIntDetail<uint16_t>();
}

uint32_t Buffer::asUInt32() const
{
    return asIntDetail<uint32_t>();
}

uint64_t Buffer::asUInt64() const
{
    return asIntDetail<uint64_t>();
}

double Buffer::asDouble() const
{
    auto s = asString();
    return atof(s.c_str());
}

float Buffer::asFloat() const
{
    return static_cast<float>(asDouble());
}

const uint8_t* Buffer::getData() const
{
    return m_data.data();
}

size_t Buffer::getLen() const
{
    return m_data.size();
}

const std::vector<uint8_t>& Buffer::getDataVector() const
{
    return m_data;
}

bool Buffer::isBuffer() const
{
    return m_type == BUFFER;
}

bool Buffer::isArray() const
{
    return m_type == ARRAY;
}

bool Buffer::empty() const
{
    return m_data.empty();
}

Buffer::TYPE Buffer::getType() const
{
    return m_type;
}

void Buffer::clearBuffer()
{
    m_data.clear();
}

void Buffer::clearArray()
{
    m_array.clear();
}

void Buffer::append(std::string s)
{
    append(s.data(), s.size());
}

void Buffer::append(const void* data, size_t len)
{
    auto p = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i != len; ++i) {
        m_data.push_back(p[i]);
    }
}

const Buffer& Buffer::operator[](size_t index) const
{
    return m_array[index];
}

Buffer& Buffer::operator[](size_t index)
{
    return m_array[index];
}

void Buffer::pushBack(Buffer&& blk)
{
    m_array.emplace_back(std::move(blk));
}

size_t Buffer::arraySize() const
{
    return m_array.size();
}

}
