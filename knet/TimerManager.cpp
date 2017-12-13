#include "knet/TimerManager.h"

#include "knet/EventManager.h"

namespace knet {

TimerManager::TimerManager(EventManager& event_manager)
    : m_event_manager(event_manager)
    , m_timers()
    , m_mtx()
    , m_wait_cb()
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
    decltype(m_wait_cb) all_wait_cb{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        std::swap(all_wait_cb, m_wait_cb);
    }

    while (!all_wait_cb.empty()) {
        TimerPtr tc = std::move(all_wait_cb.front());
        all_wait_cb.pop();

        // 如果没找到，说明此定时器已经被取消，不需要callback
        auto it = m_timers.find(tc);
        if (it == m_timers.end())
            continue;
        tc->m_sync_cb();
    }
}

TimerHdl TimerManager::AddTimer(Callback sync_cb, Duration d)
{
    auto tc = std::make_shared<Timer>();
    auto et = m_event_manager.AddTimer(std::bind(&TimerManager::EventCallback, this, tc), d);
    tc->m_sync_cb = std::move(sync_cb);
    tc->m_et = std::move(et);
    m_timers.insert(tc);
    return tc;
}

void TimerManager::CancelTimer(TimerHdl hdl)
{
    m_event_manager.CancelTimer(hdl->m_et);
    m_timers.erase(hdl);
}

void TimerManager::EventCallback(TimerHdl hdl)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_wait_cb.push(hdl);
}

} // knet
