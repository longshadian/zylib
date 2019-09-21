#include "network/IOContextPool.h"

namespace network
{

IOContext::IOContext(std::int32_t index)
    : m_index(index)
    , m_thread()
    , m_ioctx()
    , m_work(boost::asio::make_work_guard(m_ioctx))
{
    std::thread temp(std::bind(&IOContext::Run, this));
    m_thread = std::move(temp);
}

IOContext::~IOContext()
{
    Stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void IOContext::Stop()
{
    m_work.reset();
    if (!m_ioctx.stopped())
        m_ioctx.stop();
}

void IOContext::Run()
{
    while (1) {
        try {
            m_ioctx.run();
            break;
        } catch (const std::exception& e) {
            // TODO
            (void)e;
            m_ioctx.restart();
        }
    }
}

/**
 * class IOContextPool
 ******************************************************************************************/
IOContextPool::IOContextPool()
    : m_next_index()
    , m_iocxt_vec()
{
}

IOContextPool::~IOContextPool()
{
}

void IOContextPool::Init(std::int32_t count)
{
    if (count <= 0)
        count = 1;
    for (std::int32_t i = 0; i != count; ++i) {
        auto ioctx = std::make_shared<IOContext>(i);
        m_iocxt_vec.emplace_back(ioctx);
    }
}

void IOContextPool::Stop()
{
    m_iocxt_vec.clear();
}

IOContextPtr IOContextPool::NextIOContext()
{
    if (m_iocxt_vec.empty())
        return nullptr;
    auto idx = (++m_next_index) % m_iocxt_vec.size();
    return m_iocxt_vec[idx];
}

} // namespace network

