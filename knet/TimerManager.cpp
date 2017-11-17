#include "knet/TimerManager.h"

namespace knet {

TimerManager::TimerManager()
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

}

TimerHdl TimerManager::AddTimer(TimerCB cb, std::chrono::milliseconds tm)
{
    return SetTimer(std::move(cb), tm);
}

void TimerManager::CancelTimer(const TimerHdl& hdl)
{
    RemoveTimerContext(hdl);
}

void TimerManager::ThreadRun()
{
    try {
        while (m_running) {
            m_io_service.run();
        }
    } catch (const std::exception& e) {
        LOG(WARNING) << "boost io_service exception: " << e.what();
    }
}

TimerContextPtr TimerManager::SetTimer(TimerCB cb, std::chrono::milliseconds dt)
{
    auto tc = std::make_shared<TimerContext>();
    tc->m_timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
    tc->m_timer->expires_from_now(dt);
    tc->m_cb = std::move(cb);

    //timer->expires_from_now(boost::posix_time::seconds(seconds));
    InsertTimerContext(tc);

    tc->m_timer->async_wait([this, tc](const boost::system::error_code& ec) {
        // TODO 判断是否是取消
        RemoveTimerContext(tc);
        tc->m_cb();
    });
    return tc;
}

void TimerManager::InsertTimerContext(const TimerContextPtr& tc)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timers.insert(tc);
}

void TimerManager::RemoveTimerContext(const TimerContextPtr& tc)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_timers.erase(tc);
}

} // knet
