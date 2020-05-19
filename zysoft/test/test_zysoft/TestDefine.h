#pragma once

#include <cstdio>
#include <ctime>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "zysoft/utility.h"
#include "zysoft/string/md5.h"
#include "zysoft/string/base64.h"

int TestMD5();

bool IsEqual(const std::vector<std::uint8_t>& s1, const std::vector<std::uint8_t>& s2);
bool IsEqual(const void* s1, std::size_t s1_len, const void* s2, std::size_t s2_len);

class PerformanceTimer
{
public:
    PerformanceTimer()
        : m_start(), m_end()
    {
        Reset();
    }

    ~PerformanceTimer() = default;
    PerformanceTimer(const PerformanceTimer&) = default;
    PerformanceTimer& operator=(const PerformanceTimer&) = default;

    void Reset()
    {
        m_start = std::chrono::steady_clock::now();
        m_end = m_start;
    }

    void Stop()
    {
        m_end = std::chrono::steady_clock::now();
    }

    template <typename T>
    T Cost() const
    {
        return std::chrono::duration_cast<T>(m_end - m_start);
    }

    std::int64_t CostMicroseconds() const
    {
        return Cost<std::chrono::microseconds>().count();
    }

    std::int64_t CostMilliseconds() const
    {
        return Cost<std::chrono::milliseconds>().count();
    }

    std::int64_t CostSeconds() const
    {
        return Cost<std::chrono::seconds>().count();
    }

    std::chrono::steady_clock::time_point m_start;
    std::chrono::steady_clock::time_point m_end;
};


