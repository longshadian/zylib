#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <functional>

namespace zysoft
{
namespace net
{

class EventLoop;

class EventLoopThread
{
public:
    using ThreadStartCallback = std::function<void(EventLoopThread*)>;
public:
    EventLoopThread(ThreadStartCallback cb = ThreadStartCallback())
        : running_()
        , callback_(std::move(cb))
        , thread_()
        , loop_()   // 不能在此创建，必须在线程函数中创建此对象
    {
    }

    ~EventLoopThread()
    {
        if (running_ && loop_) {
            loop_->Quit();
        }
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    EventLoopThread(const EventLoopThread&) = delete;
    EventLoopThread& operator=(const EventLoopThread&) = delete;
    EventLoopThread(EventLoopThread&&) = delete;
    EventLoopThread& operator=(EventLoopThread&&) = delete;

    void Start()
    {
        if (running_.exchange(true)) {
            return;
        }
        thread_ = std::thread(std::bind(&EventLoopThread::ThreadFunc, this));
    }

    EventLoop* GetEventLoop()
    {
        return loop_.get();
    }

private:
    void ThreadFunc()
    {
        if (callback_) 
            callback_;

        loop_ = std::make_unique<EventLoop>();
        loop_->Loop();
    }

private:
    std::atomic<bool>   running_;
    ThreadStartCallback callback_;
    std::thread thread_;
    std::unique_ptr<EventLoop> loop_;
};

} // namespace net
} // namespace zysoft

