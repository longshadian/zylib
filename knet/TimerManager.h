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

class TimerContext
{
public:
    TimerContext();
    ~TimerContext();
    TimerContext(const TimerContext&) = delete;
    TimerContext& operator=(const TimerContext&) = delete;
    TimerContext(TimerContext&&) = delete;
    TimerContext& operator=(TimerContext&&) = delete;

};

using TimerContextPtr = std::shared_ptr<TimerContext>;
using TimerHdl = std::shared_ptr<TimerContext>;
using TimerCB = std::function<void()>;


class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;
    TimerManager(TimerManager&&) = delete;
    TimerManager& operator=(TimerManager&&) = delete;

    bool Init();
    void Tick(DiffTime diff);
    void addTimer(TimerCB cb, std::chrono::milliseconds tm);

private:
    void ThreadRun();
    TimerContextPtr SetTimer(std::chrono::milliseconds dt);

private:
    boost::asio::io_service m_io_service;
    std::unique_ptr<boost::asio::io_service::work> m_ios_work;
    std::atomic<bool>       m_running;
    std::thread             m_thread;
    std::mutex              m_mtx;
    std::unordered_set<TimerContextPtr> m_timers;
};

} // knet
