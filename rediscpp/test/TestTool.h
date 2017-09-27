#pragma once

#include <cassert>
#include <cstring>
#include <vector>
#include <type_traits>

#include "rediscpp.h"

#define TEST assert

void pout(const std::vector<rediscpp::Buffer>& v);
void pout(const rediscpp::Buffer& v);

void pout(const rediscpp::BufferArray& v);
void poutArrayCell(const rediscpp::BufferArray& v);

namespace test_tool {

template <typename T>
void bzero(T* v)
{
    static_assert(std::is_pod<T>::value, "xxx ");
    std::memset(v, 0, sizeof(T));
}

struct Timer
{
    Timer()
    {
        m_start = std::chrono::system_clock::now();
        m_end = m_start;
    }

    ~Timer()
    {
    }

    void end()
    {
        m_end = std::chrono::system_clock::now();
    }

    uint64_t cost() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
    }

    std::chrono::system_clock::time_point m_start;
    std::chrono::system_clock::time_point m_end;
};

}
