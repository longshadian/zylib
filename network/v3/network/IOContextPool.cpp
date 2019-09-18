#include "network/IOContextPool.h"

IOContext::IOContext(std::int32_t index)
    : m_index(index)
    , m_thread()
    , m_io_context()
    , m_work(boost::asio::make_work_guard(m_io_context))
{
    std::thread temp(std::bind(&IOContext::Run, this));
    m_thread = std::move(temp);
}

IOContext::~IOContext()
{
    m_work.reset();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void IOContext::Run()
{
    while (1) {
        try {
            m_io_context.run();
            break;
        } catch (const std::exception& e) {
            // TODO
            (void)e;
            m_io_context.restart();
        }
    }
}

/**
 * class IOContextPool
 ******************************************************************************************/
IOContextPool::IOContextPool()
{
}

IOContextPool::~IOContextPool()
{
}

bool IOContextPool::Init(std::int32_t count)
{
    if (count <= 0)
        count = 1;
    for (std::int32_t i = 0; i != count; ++i) {
        auto ioctx = std::make_shared<IOContext>(i);
        m_slots.emplace_back(ioctx);
    }
    return true;
}

void IOContextPool::Stop()
{
    m_slots.clear();
}

IOContextPtr IOContextPool::NextIOContext()
{
    if (m_slots.empty())
        return nullptr;
    auto idx = m_next_index % m_slots.size();
    ++m_next_index;
    return m_slots[idx];
}

