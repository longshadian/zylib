#ifndef _TIMER_SERVICE_H_
#define _TIMER_SERVICE_H_

#include <cstdint>
#include <string>
#include <list>
#include <memory>
#include <functional>

using TimerTick = uint32_t;

class TimerService
{
    struct TimerTask
    {
        std::function<void()> m_fun;
        TimerTick             m_time;
    };
public:
    TimerService();
    ~TimerService() = default;

    bool init();
    void heartbeat(uint32_t diff);
    void addTimer(std::function<void()> fun, TimerTick tk);
private:
    void timerRotation(bool call);
private:
    TimerTick            m_time_passed;
    std::list<TimerTask> m_timers;
};

#endif
