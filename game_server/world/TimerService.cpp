#include "TimerService.h"

#include <algorithm>

TimerService::TimerService()
    : m_time_passed()
    , m_timers()
{
}

bool TimerService::init()
{
    return true;
}

void TimerService::addTimer(std::function<void()> fun, TimerTick tk)
{
    TimerTask timer = {fun, tk};
    if (m_time_passed != 0) {
        timerRotation(false);
        m_time_passed = 0;
    }
    auto it = std::lower_bound(m_timers.begin(), m_timers.end(), timer,
        [](const TimerTask& it, const TimerTask& val)
        {
            return it.m_time < val.m_time;
        });
    m_timers.insert(it, timer);
}

void TimerService::heartbeat(uint32_t diff)
{
    if (m_timers.empty())
        return;
    m_time_passed += diff;
    if (m_time_passed < m_timers.front().m_time)
        return;
    timerRotation(true);
    m_time_passed = 0;
}

void TimerService::timerRotation(bool call)
{
    if (m_time_passed == 0)
        return;
    for (auto it = m_timers.begin(); it != m_timers.end();) {
        if (it->m_time <= m_time_passed) {
            it->m_time = 0;
            if (call) {
                it->m_fun();
                it = m_timers.erase(it);
            } else {
                ++it;
            }
        } else {
            it->m_time -= m_time_passed;
            ++it;
        }
    }
}
