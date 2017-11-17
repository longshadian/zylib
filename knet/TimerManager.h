#pragma once

#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <unordered_set>

#include <boost/asio.hpp>

#include "knet/KNetTypes.h"

namespace knet {

using TimerCB = std::function<void()>;

class TimerContext
{
public:
    TimerContext();
    ~TimerContext();
    TimerContext(const TimerContext&) = delete;
    TimerContext& operator=(const TimerContext&) = delete;
    TimerContext(TimerContext&&) = delete;
    TimerContext& operator=(TimerContext&&) = delete;

    std::unique_ptr<boost::asio::deadline_timer> m_timer;
    TimerCB m_cb;
};

using TimerContextPtr = std::shared_ptr<TimerContext>;
using TimerHdl = std::shared_ptr<TimerContext>;


class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;
    TimerManager(TimerManager&&) = delete;
    TimerManager& operator=(TimerManager&&) = delete;

    // TODO 转换成同步执行
    bool Init();
    void Tick(DiffTime diff);
    TimerHdl AddTimer(TimerCB cb, std::chrono::milliseconds tm);
    void CancelTimer(const TimerHdl& hdl);

private:
    void ThreadRun();
    TimerContextPtr SetTimer(TimerCB cb, std::chrono::milliseconds dt);
    void InsertTimerContext(const TimerContextPtr& tc);
    void RemoveTimerContext(const TimerContextPtr& tc);

private:
    boost::asio::io_service m_io_service;
    std::unique_ptr<boost::asio::io_service::work> m_ios_work;
    std::atomic<bool>       m_running;
    std::thread             m_thread;
    std::mutex              m_mtx;
    std::unordered_set<TimerContextPtr> m_timers;
};

} // knet
