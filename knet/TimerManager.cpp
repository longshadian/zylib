#include "knet/TimerManager.h"

#include "knet/EventManager.h"

namespace knet {

void Timer::Call()
{
    m_sync_cb();
}

void Timer::SetCallback(Callback cb)
{
    m_sync_cb = std::move(cb);
}

void Timer::SetEventTimer(EventTimerPtr et)
{
    m_et = std::move(et);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TimerManager::TimerManager(EventManager& event_manager)
    : m_event_manager(event_manager)
    , m_timers()
    , m_mtx()
    , m_timeout_timers()
{
}

TimerManager::~TimerManager()
{
}

bool TimerManager::Init()
{
    return true;
}

void TimerManager::Tick(DiffTime diff)
{
    (void)diff;
    decltype(m_timeout_timers) all{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all, m_timeout_timers);
    }

    while (!all.empty()) {
        TimerHdl hdl = std::move(all.front());
        all.pop();
        auto timer = hdl.lock();
        if (timer) {
            timer->Call();
            m_timers.erase(timer);
        }
        // 如果没找到，说明此定时器已经被取消，不需要callback
    }
}

TimerHdl TimerManager::AddTimer(Callback sync_cb, Duration d)
{
    auto timer = std::make_shared<Timer>();
    auto et = m_event_manager.AddTimer(std::bind(&TimerManager::CB_Event, this, TimerHdl{timer}), d);
    timer->SetCallback(std::move(sync_cb));
    timer->SetEventTimer(std::move(et));
    m_timers.insert(timer);
    return TimerHdl{timer};
}

void TimerManager::CancelTimer(TimerHdl hdl)
{
    auto timer = hdl.lock();
    if (!timer)
        return;
    m_event_manager.CancelTimer(timer->m_et);
    m_timers.erase(timer);
}

void TimerManager::CB_Event(TimerHdl hdl)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timeout_timers.push(std::move(hdl));
}

} // knet
