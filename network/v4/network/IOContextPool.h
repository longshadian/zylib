#pragma once

#include <cstdint>
#include <thread>
#include <atomic>
#include <memory>
#include <boost/asio.hpp>

class IOContext
{
public:
    IOContext(std::int32_t index);
    ~IOContext();
    IOContext(const IOContext& rhs) = delete;
    IOContext& operator=(const IOContext& rhs) = delete;
    IOContext(IOContext&& rhs) = delete;
    IOContext& operator=(IOContext&& rhs) = delete;

    std::int32_t                        m_index;
    std::thread                         m_thread;
    boost::asio::io_context             m_ioctx;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work;

    void Stop();

private:
    void Run();
};
using IOContextPtr = std::shared_ptr<IOContext>;

class IOContextPool
{
public:
    IOContextPool();
    ~IOContextPool();
    IOContextPool(const IOContextPool& rhs) = delete;
    IOContextPool& operator=(const IOContextPool& rhs) = delete;
    IOContextPool(IOContextPool&& rhs) = delete;
    IOContextPool& operator=(IOContextPool&& rhs) = delete;

    void                            Init(std::int32_t count);
    void                            Stop();
    IOContextPtr                    NextIOContext();
private:
    std::atomic<std::uint64_t>      m_next_index;
    std::vector<IOContextPtr>       m_iocxt_vec;
};
