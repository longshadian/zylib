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

void TimerManager::addTimer(TimerCB cb, std::chrono::milliseconds tm)
{
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

TimerContextUPtr TimerManager::SetTimer(std::chrono::milliseconds dt)
{
    auto tc = std::make_unique<TimerContext>();
    tc->m_timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
    tc->m_timer->expires_from_now(dt);
    //timer->expires_from_now(boost::posix_time::seconds(seconds));
    m_timers.insert(std::);

    tc->m_timer->async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            self->onClosed(CLOSED_TYPE::TIMEOUT);
        }
    });
    return timer;
}

} // knet
