#include "knet/EventManager.h"

namespace knet {

EventTask::EventTask(EventID id, Callback sync, Callback async)
    : m_id(id)
    , m_sync_cb(std::move(sync))
    , m_async_cb(std::move(async))
{

}

EventTask::~EventTask()
{

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
SocketPair::SocketPair(boost::asio::io_service io_service)
    : m_io_service(io_service)
    , m_sock_read(m_io_service)
    , m_sock_write(m_io_service)
    , m_read_buf()
    , m_write_buf()
{
}

SocketPair::~SocketPair()
{
    try {
        m_sock_write.shutdown(boost::asio::socket_base::shutdown_both);
        m_sock_write.close();
        m_sock_read.shutdown(boost::asio::socket_base::shutdown_both);
        m_sock_read.close();
    } catch (const std::exception& e) {
        (void)e;
    }
}

void SocketPair::AsyncRead()
{
    boost::asio::async_read(m_sock_read, boost::asio::buffer(m_read_buf),
        [this](boost::system::error_code ec, size_t length)
        {
            if (ec) {
                LOG(WARNING) << "socket read error. " << ec.message();
                return;
            }
            EventTask em{};
            std::memcpy(&em, m_read_buf.data(), m_read_buf.size());
            // TODO callback em;

            async_read();
        });
}

void SocketPair::AsyncWrite(const EventTask& msg)
{
    m_io_service.post([this, buf = msg.serializeToBinary()]()
    {
        bool wait_write = !m_write_buf.empty();
        m_write_buf.push_back(std::move(buf));
        if (!wait_write) {
            DoWrite();
        }
    });
}

void SocketPair::DoWrite()
{
    boost::asio::async_write(m_sock_write
        , boost::asio::buffer(m_write_buf.front().data(), m_write_buf.front().size())
        , [this](boost::system::error_code ec, size_t length)
        {
            if (ec) {
                LOG(WARNING) << "write error. " << ec.message();
                return;
           }
        }
    );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
EventManager::EventManager()
    : m_io_service()
    , m_ios_work(std::make_unique<boost::asio::io_service::work>(m_io_service))
    , m_running()
    , m_thread()
    , m_next_event_id()
    , m_mtx()
    , m_socket_pair(std::make_unique<SocketPair>(m_io_service))
{
    m_socket_pair->AsyncRead();
}

EventManager::~EventManager()
{
    if (!m_io_service.stopped())
        m_io_service.stop();
    if (m_thread.joinable())
        m_thread.join();
}

bool EventManager::Init()
{
    if (m_running.exchange(true))
        return false;
    std::thread temp_thread{std::bind(&EventManager::ThreadRun, this)};
    m_thread = std::move(temp_thread);
    return true;
}

void EventManager::Shutdown()
{
    if (!m_io_service.stopped())
        m_io_service.stop();
}

void EventManager::Tick(DiffTime diff)
{
    (void)diff;
}

EventTimerPtr EventManager::AddTimer(Callback async_cb, Duration d)
{
    auto et = std::make_shared<EventTimer>();
    et->m_timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
    et->m_timer->expires_from_now(d);
    et->m_async_cb = std::move(async_cb);
    et->m_timer->async_wait([this, et](const boost::system::error_code& ec) 
        {
            if (ec)
                return;
            et->m_async_cb();
        });
    return et;
}

void EventManager::CancelTimer(EventTimerPtr& et)
{
    try {
        et->m_timer->cancel();
    } catch (const std::exception& e) {
        (void)e;
    }
}

void EventManager::AddEvent(Callback sync_cb, Callback async_cb)
{
    auto event_task = std::make_shared<EventTask>(++m_next_event_id, std::move(sync_cb), std::move(async_cb));
}

void EventManager::ThreadRun()
{
    try {
        m_io_service.run();
    } catch (const std::exception& e) {
        LOG(WARNING) << "boost io_service exception: " << e.what();
    }
}

} // knet
