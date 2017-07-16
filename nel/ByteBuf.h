#pragma once

#include <exception>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <time.h>
#include <cmath>
#include <type_traits>
#include <cassert>
#include <iostream>

namespace nlnet {

class ByteBufException : public std::runtime_error
{
public:
    ByteBufException(const std::string& str) 
        : std::runtime_error(str)
    {
    }

    ~ByteBufException() throw() { }
};

class ByteBufOverflowException : public ByteBufException
{
public:
    ByteBufOverflowException(size_t pos, size_t size, size_t remian_size);
    ~ByteBufOverflowException() throw() { }

private:
    static std::string toString(size_t pos, size_t size, size_t remain_size);
};

class ByteBuf
{
public:
    static const size_t DEFAULT_SIZE = 1024 * 4;

    ByteBuf();
    ByteBuf(size_t res);
    ByteBuf(std::vector<uint8_t> buf);
    ByteBuf(const void* src, size_t len);
    ~ByteBuf();

    ByteBuf(ByteBuf&& buf);
    ByteBuf(const ByteBuf& right);

    ByteBuf& operator=(const ByteBuf& right);
    ByteBuf& operator=(ByteBuf&& right);

    size_t byteSize() const;
    bool byteEmpty() const;
    void resize(size_t s);
    void reserve(size_t s);
    void clear();
    const uint8_t* data() const;

public:
    template <typename T>
    ByteBuf& operator<<(T value)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
        append(&value, sizeof(value));
        return *this;
    }

    ByteBuf& operator<<(bool value)
    {
        uint8_t v = value ? 1 : 0;
        (*this) << v;
        return *this;
    }

    ByteBuf& operator<<(const std::string& value)
    {
        if (size_t len = value.length())
            append(value.c_str(), len);
        return *this;
    }

    ByteBuf& operator<<(const char* str)
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
    ByteBuf& operator>>(T& val)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
        read(&val); readSkip<T>();
        return *this;
    }

    ByteBuf& operator>>(std::string& val)
    {
        val.clear();
        while (!byteEmpty()) {
            char c{};
            read(&c); readSkip<char>();
            val.push_back(c);
        }
        return *this;
    }

    ByteBuf& operator>>(std::vector<uint8_t>& val)
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
            throw ByteBufException("read float isfinite == false");
    }

    void read(double* val)
    {
        read(val, sizeof(double));
        if (!std::isfinite(*val))
            throw ByteBufException("read double isfinite == false");
    }

    void read(long double* val)
    {
        read(val, sizeof(long double));
        if (!std::isfinite(*val))
            throw ByteBufException("read long double isfinite == false");
    }

    void read(void* dest, size_t len)
    {
        if (len > byteSize())
            throw ByteBufOverflowException(m_rpos, len, byteSize()); 
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

} // nlnet
