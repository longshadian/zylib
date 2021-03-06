#pragma once

#include "Hiredis.h"

#include <string>
#include <vector>
#include <memory>

#include "Convert.h"

namespace rediscpp {

class Buffer
{
public:
    Buffer();
    ~Buffer() = default;

    explicit Buffer(const char* p);
    explicit Buffer(const std::string& str);
    explicit Buffer(const char* p, size_t len);
    explicit Buffer(std::vector<uint8_t> data);
    explicit Buffer(int val);
    explicit Buffer(unsigned int val);
    explicit Buffer(long val);
    explicit Buffer(unsigned long val);
    explicit Buffer(long long val);
    explicit Buffer(unsigned long long val);
    explicit Buffer(float val);
    explicit Buffer(double val);
    explicit Buffer(long double val);

    Buffer(const Buffer& rhs);
    Buffer& operator=(const Buffer& rhs);

    Buffer(Buffer&& rhs);
    Buffer& operator=(Buffer&& rhs);

public:
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
    const std::vector<uint8_t>& getDataVector() const &;
    std::vector<uint8_t> getDataVector() && ;
    bool empty() const;
    void clearBuffer();

    void append(std::string s);
    void append(const void* data, size_t len);
private:
    template <typename T>
    T asIntDetail() const
    {
        auto s = asString();
        if (s.empty())
            return T{};
        return detail::Convert<T>::cvt_noexcept(s);
    }
private:
    std::vector<uint8_t>    m_data;
};

class BufferArray
{
public:
    enum class TYPE 
    {
        BUFFER = 0,
        ARRAY = 1,
    };

    using array_type    = std::vector<BufferArray>;
    using iterator      = array_type::iterator;
    using const_iterator = array_type::const_iterator;      

private:
    BufferArray(TYPE t);

public:
    ~BufferArray() = default;
    BufferArray(const BufferArray& rhs); 
    BufferArray& operator=(const BufferArray& rhs); 
    BufferArray(BufferArray&& rhs); 
    BufferArray& operator=(BufferArray&& rhs); 

public:
    static BufferArray initBuffer();
    static BufferArray initArray();

    bool isBuffer() const;
    bool isArray() const;

    //BUFFER ����
    void setBuffer(Buffer b);
    Buffer& getBuffer()&;
    const Buffer& getBuffer() const &;
    Buffer getBuffer() &&;

public:
    //ARRAY ����

    void                push_back(Buffer b);
    void                push_back(BufferArray a);
    BufferArray         pop_back();

    BufferArray&        operator[](size_t idx) &;
    const BufferArray&  operator[](size_t idx) const &;
    BufferArray         operator[](size_t idx) &&;

    size_t              size() const;
    bool                empty() const;
    iterator            begin();
    iterator            end();
    const_iterator      begin() const;
    const_iterator      end() const;
    const_iterator      cbegin() const;
    const_iterator      cend() const;
private:
    void checkType(TYPE t) const;
private:
    TYPE                     m_type;
    Buffer                   m_buffer;
    std::vector<BufferArray> m_array;
};

}
