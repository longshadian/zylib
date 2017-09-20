#pragma once

#include <cstring>
#include <cstdint>
#include <cmath>

#include <stdexcept>
#include <string>
#include <vector>
#include <type_traits>

namespace network {

class ByteBufferException : public std::runtime_error
{
public:
    ByteBufferException(const std::string& str) 
        : std::runtime_error(str)
    {
    }

    ~ByteBufferException() throw() { }
};

class ByteBufferOverflowException : public ByteBufferException
{
public:
    ByteBufferOverflowException(size_t pos, size_t size, size_t remian_size);
    ~ByteBufferOverflowException() throw() { }

private:
    static std::string toString(size_t pos, size_t size, size_t remain_size);
};

class ByteBuffer
{
public:
    static const size_t DEFAULT_SIZE = 1024 * 4;

    ByteBuffer();
    ByteBuffer(size_t res);
    ByteBuffer(std::vector<uint8_t> buf);
    ByteBuffer(const void* src, size_t len);
    ~ByteBuffer();

    ByteBuffer(ByteBuffer&& buf);
    ByteBuffer(const ByteBuffer& right);

    ByteBuffer& operator=(const ByteBuffer& right);
    ByteBuffer& operator=(ByteBuffer&& right);

    size_t byteSize() const;
    bool byteEmpty() const;
    void resize(size_t s);
    void reserve(size_t s);
    void clear();
    const uint8_t* data() const;

public:
    template <typename T>
    ByteBuffer& operator<<(T value)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
        append(&value, sizeof(value));
        return *this;
    }

    ByteBuffer& operator<<(bool value)
    {
        uint8_t v = value ? 1 : 0;
        (*this) << v;
        return *this;
    }

    ByteBuffer& operator<<(const std::string& value)
    {
        if (size_t len = value.length())
            append(value.c_str(), len);
        return *this;
    }

    ByteBuffer& operator<<(const char* str)
    {
        if (size_t len = (str ? strlen(str) : 0))
            append(str, len);
        return *this;
    }

    void append(const void* src, size_t cnt)
    {
        if (cnt == 0)
            return;
        if (!src)
            return;
        if (m_storage.size() < m_wpos + cnt)
            m_storage.resize(m_wpos + cnt);
        std::memcpy(&m_storage[m_wpos], src, cnt);
        m_wpos += cnt;
    }

public:

    template <typename T>
    ByteBuffer& operator>>(T& val)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
        read(&val); readSkip<T>();
        return *this;
    }

    ByteBuffer& operator>>(std::string& val)
    {
        val.clear();
        while (!byteEmpty()) {
            char c{};
            read(&c); readSkip<char>();
            val.push_back(c);
        }
        return *this;
    }

    ByteBuffer& operator>>(std::vector<uint8_t>& val)
    {
        val.clear();
        if (!byteEmpty()) {
            val.assign(data(), data() + byteSize());
            readSkip(val.size());
        }
        return *this;
    }

public:

    template <typename T> 
    void read(T* val)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
        read(val, sizeof(T));
    }

    void read(bool* val)
    {
        uint8_t v{};
        read(&v, sizeof(v));
        *val = (v != 0);
    }

    void read(float* val)
    {
        read(val, sizeof(float));
        if (!std::isfinite(*val))
            throw ByteBufferException("read float isfinite == false");
    }

    void read(double* val)
    {
        read(val, sizeof(double));
        if (!std::isfinite(*val))
            throw ByteBufferException("read double isfinite == false");
    }

    void read(long double* val)
    {
        read(val, sizeof(long double));
        if (!std::isfinite(*val))
            throw ByteBufferException("read long double isfinite == false");
    }

    void read(void* dest, size_t len)
    {
        if (len > byteSize())
            throw ByteBufferOverflowException(m_rpos, len, byteSize()); 
        std::memcpy(dest, &m_storage[m_rpos], len);
    }

    template <typename T>
    void readSkip()
    {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
        readSkip(sizeof(T));
    }

    void readSkip(size_t skip);

    void shrinkToFit();
protected:
    size_t m_rpos;
    size_t m_wpos;
    std::vector<uint8_t> m_storage;
};

} // network
