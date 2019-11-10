#pragma once

#include <chrono>
#include <functional>

namespace zylib {

template <typename T>
class BasicTimer
{
public:
	using Duration = T;

public:
    BasicTimer() = default;
    ~BasicTimer() = default;
    BasicTimer(const BasicTimer&) = default;
    BasicTimer& operator=(const BasicTimer&) = default;

	template <typename D>
	BasicTimer(D d) 
        : m_flag() , m_start() , m_expire(d)
	{
	}

	void Update(Duration delta)
	{
		m_start += delta;
	}

	bool Passed() const
	{
		return m_expire <= m_start;
	}

	void Reset()
	{
		m_start = Duration::zero();
        m_flag = 0;
	}

	Duration Remain() const
	{
		return m_expire - m_start;
	}

    void SetFlag(std::int32_t flag)
    {
        m_flag = flag;
    }

    std::int32_t GetFalg() const 
    { 
        return m_flag; 
    }

    std::int32_t    m_flag;
	Duration        m_start;
	Duration        m_expire;
};

using SimpleTimer = BasicTimer<std::chrono::milliseconds>;
using Milliseconds = std::chrono::milliseconds;

using TimePoint = std::chrono::steady_clock::time_point;
inline TimePoint SteadyTimePoint()
{
	return std::chrono::steady_clock::now();
}

inline Milliseconds GetDelta(TimePoint b, TimePoint e)
{
	return std::chrono::duration_cast<Milliseconds>(e - b);
}


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


} // namespace zylib

