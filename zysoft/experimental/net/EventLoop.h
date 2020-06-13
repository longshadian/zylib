#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include <boost/asio.hpp>

#include <net/console_log.h>

namespace zysoft
{
namespace net
{

class EventLoop
{
public:
    using Functor = std::function<void()>;

public:
    EventLoop()
        : io_ctx_()
        , looping_()
        , thread_id_(std::this_thread::get_id())
    {
    }

    ~EventLoop()
    {
        Quit();
    }

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;

    void Loop()
    {
        assert(!looping_);
        looping_ = true;

        boost::asio::io_context::work work_guard{io_ctx_};
        while (looping_) {
            try {
                io_ctx_.run();
                NET_LOG_TRACE << "io_context exit";
                break;
            } catch (const std::exception& e) {
                NET_LOG_TRACE << "io_content exception: " << e.what();
                io_ctx_.restart();
            }
        }
    }

    void Quit()
    {
        looping_ = false;
        io_ctx_.stop();
    }

    void RunInLoop(Functor f)
    {
        io_ctx_.post(std::move(f));
    }

    boost::asio::io_context& GetIOContext()
    {
        return io_ctx_;
    }

    bool IsInLoopThread() const
    {
        return thread_id_ == std::this_thread::get_id();
    }

private:
    boost::asio::io_context io_ctx_;
    //boost::asio::io_context::work work_guard_;
    std::atomic<bool> looping_;
    const std::thread::id thread_id_;
};

} // namespace net
} // namespace zysoft


