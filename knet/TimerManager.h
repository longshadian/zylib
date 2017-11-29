#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>
#include <queue>

#include "knet/KNetTypes.h"

namespace knet {

class EventManager;

class TimerContext
{
public:
    TimerContext()
        : m_sync_cb()
        , m_et() 
    {}

    ~TimerContext() {}
    TimerContext(const TimerContext&) = delete;
    TimerContext& operator=(const TimerContext&) = delete;
    TimerContext(TimerContext&&) = delete;
    TimerContext& operator=(TimerContext&&) = delete;

    Callback        m_sync_cb;
    EventTimerPtr   m_et;
};

class TimerManager
{
public:
    TimerManager(EventManager& event_manager);
    ~TimerManager();
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;
    TimerManager(TimerManager&&) = delete;
    TimerManager& operator=(TimerManager&&) = delete;

    bool Init();
    void Tick(DiffTime diff);
    TimerHdl AddTimer(Callback sync_cb, Duration d);
    void CancelTimer(TimerHdl hdl);

private:
    void EventCallback(TimerHdl hdl);

private:
    EventManager&                       m_event_manager;
    std::unordered_set<TimerContextPtr> m_timers;
    std::mutex                          m_mtx;
    std::queue<TimerContextPtr>         m_wait_cb;
};

} // knet
