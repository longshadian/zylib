#pragma once

#include <chrono>
#include <functional>

namespace zylib {

using namespace std::chrono;

template <typename T>
struct BasicTimer
{
	using Duration = T;

	BasicTimer() = default;

	template <typename D>
	BasicTimer(D d)
		: m_valid(true)
        , m_start()
		, m_expire(d)
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
	}

	Duration Remain() const
	{
		return m_expire - m_start;
	}

    bool Valid() const { return m_valid; }

    bool     m_valid;
	Duration m_start;
	Duration m_expire;
};

using TimingWheel = BasicTimer<milliseconds>;
using Delta = milliseconds;

using TimePoint = std::chrono::steady_clock::time_point;

inline TimePoint SteadyTimePoint()
{
	return std::chrono::steady_clock::now();
}

inline Delta GetDelta(TimePoint b, TimePoint e)
{
	return std::chrono::duration_cast<Delta>(e - b);
}

//////////////////////////////////////////////////////////////////////////
}

