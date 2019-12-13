#pragma once

#include <cstdint>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <functional>

#include <boost/asio.hpp>

#include "network/FakeLog.h"

namespace network
{

class IOContext
{
public:
    IOContext(std::int32_t index)
        : m_index(index)
        , m_thread()
        , m_ioctx()
        , m_work(boost::asio::make_work_guard(m_ioctx))
    {
        std::thread temp(std::bind(&IOContext::Run, this));
        m_thread = std::move(temp);
    }

    ~IOContext()
    {
        m_work.reset();
        if (!m_ioctx.stopped())
            m_ioctx.stop();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    IOContext(const IOContext& rhs) = delete;
    IOContext& operator=(const IOContext& rhs) = delete;
    IOContext(IOContext&& rhs) = delete;
    IOContext& operator=(IOContext&& rhs) = delete;

    std::int32_t                        m_index;
    std::thread                         m_thread;
    boost::asio::io_context             m_ioctx;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work;

    void Stop()
    {
        m_work.reset();
        if (!m_ioctx.stopped())
            m_ioctx.stop();
    }

private:
    void Run()
    {
        while (1) {
            try {
                m_ioctx.run();
                NETWORK_DPrintf("ioconentxt: %d stop running", m_index);
                break;
            } catch (const std::exception& e) {
                NETWORK_WPrintf("iocontext: %d exception: %s", m_index, e.what());
                m_ioctx.restart();
            }
        }
    }
};
using IOContextPtr = std::shared_ptr<IOContext>;

class IOContextPool
{
public:
    IOContextPool()
        : m_next_index()
        , m_ioc_vec()
    {
    }

    ~IOContextPool()
    {
    }

    IOContextPool(const IOContextPool& rhs) = delete;
    IOContextPool& operator=(const IOContextPool& rhs) = delete;
    IOContextPool(IOContextPool&& rhs) = delete;
    IOContextPool& operator=(IOContextPool&& rhs) = delete;

    void                            Init(std::int32_t count)
    {
        if (count <= 0)
            count = 1;
        for (std::int32_t i = 0; i != count; ++i) {
            auto ioctx = std::make_shared<IOContext>(i);
            m_ioc_vec.emplace_back(ioctx);
        }
    }

    void                            Stop()
    {
        m_ioc_vec.clear();
    }

    IOContextPtr                    NextIOContext()
    {
        if (m_ioc_vec.empty())
            return nullptr;
        auto idx = (++m_next_index) % m_ioc_vec.size();
        return m_ioc_vec[idx];
    }

private:
    std::atomic<std::uint64_t>      m_next_index;
    std::vector<IOContextPtr>       m_ioc_vec;
};

} // namespace network
