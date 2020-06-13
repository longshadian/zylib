#pragma once

#include <memory>
#include <vector>

#include <net/EventLoopThread.h>

namespace zysoft
{
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* base_loop)
        : base_loop_(base_loop)
        , threads_()
        , num_threads_(0)
        , next_(0)
    {
    }
    
    ~EventLoopThreadPool()
    {
    }

    EventLoopThreadPool(const EventLoopThreadPool&) = delete;
    EventLoopThreadPool& operator=(const EventLoopThreadPool&) = delete;
    EventLoopThreadPool(EventLoopThreadPool&&) = delete;
    EventLoopThreadPool& operator=(EventLoopThreadPool&&) = delete;

    void SetThreadNum(int num) 
    { 
        num_threads_ = num;
    }

    void Start()
    {
        for (int i = 0; i < num_threads_; ++i) {
            threads_.emplace_back(std::make_unique<EventLoopThread>());
        }
    }

    EventLoop* NextEventLoop()
    {
        if (threads_.empty()) {
            return base_loop_;
        }
        EventLoop* loop = threads_[next_]->GetEventLoop();
        ++next_;
        if (next_ >= threads_.size()) {
            next_ = 0;
        }
        return loop;
    }

private:
    EventLoop* base_loop_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    int num_threads_;
    std::size_t next_;
};


} // namespace net
} // namespace zysoft

