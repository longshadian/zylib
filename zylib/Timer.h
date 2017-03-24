#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <functional>

namespace zylib {

using namespace std::chrono;

inline 
uint32_t getMSTime()
{
    static const system_clock::time_point app_start_time = system_clock::now();

    return uint32_t(duration_cast<milliseconds>(system_clock::now() - app_start_time).count());
}

inline 
uint32_t getMSTimeDiff(uint32_t old_milliseconds_time, uint32_t new_milliseconds_time)
{
    if (old_milliseconds_time > new_milliseconds_time)
        return (std::numeric_limits<uint32_t>::max() - old_milliseconds_time) + new_milliseconds_time;
    else
        return new_milliseconds_time - old_milliseconds_time;
}

inline 
uint32_t GetMSTimeDiffToNow(uint32_t old_milliseconds_time)
{
    return getMSTimeDiff(old_milliseconds_time, getMSTime());
}

template <typename T>
struct TimerTracker
{
public:
    TimerTracker(T expiry = T())
        : m_expiry_time(expiry)
    {
    }

    void update(T diff)
    {
        if (m_expiry_time >= diff)
            m_expiry_time -= diff;
        else 
            m_expiry_time = 0;
    }

    bool passed() const
    {
        return m_expiry_time == 0;
    }

    void reset(T interval)
    {
        m_expiry_time = interval;
    }

    T getExpiry() const
    {
        return m_expiry_time;
    }
private:
    T m_expiry_time;
};

//milliseconds
typedef TimerTracker<uint32_t> MSTimerTracker;

template <typename T>
struct TimerWheel
{
    typedef std::function<void()> Function;
public:
    TimerWheel() = default;
    TimerWheel(T expiry, Function f)
        : m_init_time(expiry)
        , m_expiry_time(expiry)
        , m_fun(std::move(f))
    {
    }

    void update(T diff)
    {
        if (m_expiry_time >= diff)
            m_expiry_time -= diff;
        else 
            m_expiry_time = 0;
    }

    bool passed() const
    {
        return m_expiry_time == 0;
    }

    T getExpiry() const
    {
        return m_expiry_time;
    }

    T getInitExpiry() const
    {
        return m_init_time;
    }

    void call()
    {
        if (m_fun)
            m_fun();
    }
private:
    T           m_init_time;
    T           m_expiry_time;
    Function    m_fun;
};

//milliseconds
typedef TimerWheel<uint32_t> MSTimerWheel;

//////////////////////////////////////////////////////////////////////////
}
#endif
