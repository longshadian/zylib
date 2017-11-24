#pragma once

#include <array>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <unordered_set>
#include <list>
#include <type_traits>

#include <boost/asio.hpp>

#include "knet/KNetTypes.h"

namespace knet {

using EventID = uint64_t;

class EventTask
{
public:
    EventTask(EventID id, Callback sync, Callback async);
    ~EventTask();
    EventTask(const EventTask&) = delete;
    EventTask& operator=(const EventTask&) = delete;
    EventTask(EventTask&&) = delete;
    EventTask& operator=(EventTask&&) = delete;

    enum { Size = sizeof(EventID) };
    EventID m_event_id;

    std::vector<uint8_t> serializeToBinary() const
    {
        std::vector<uint8_t> buf{};
        buf.resize(Size);
        std::memcpy(buf.data(), &m_event_id, buf.size());
        return buf;
    }

private:
    EventID     m_id;
    Callback    m_sync_cb;
    Callback    m_async_cb;
};

class SocketPair
{
public:
    SocketPair(boost::asio::io_service io_service);
    ~SocketPair();
    SocketPair(const SocketPair&) = delete;
    SocketPair& operator=(const SocketPair&) = delete;
    SocketPair(SocketPair&&) = delete;
    SocketPair& operator=(SocketPair&&) = delete;

    void AsyncRead();
    void AsyncWrite(const EventTask& msg);

private:
    void DoWrite();

private:
    boost::asio::io_service& m_io_service;
    boost::asio::local::stream_protocol::socket m_sock_read;
    boost::asio::local::stream_protocol::socket m_sock_write;

    std::array<uint8_t, EventTask::Size>    m_read_buf;
    std::list<std::vector<uint8_t>>         m_write_buf;
};

struct EventTimer
{
    std::shared_ptr<boost::asio::deadline_timer> m_timer;
    Callback m_async_cb;
};
using EventTimerPtr = std::shared_ptr<EventTimer>;

class EventManager
{
public:
    EventManager();
    ~EventManager();
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
    EventManager(EventManager&&) = delete;
    EventManager& operator=(EventManager&&) = delete;

    bool Init();
    void Shutdown();
    void Tick(DiffTime diff);
    EventTimerPtr AddTimer(Callback async_cb, Duration d);
    void CancelTimer(EventTimerPtr& et);

    void AddEvent(Callback sync_cb, Callback async_cb);

private:
    void ThreadRun();

private:
    boost::asio::io_service     m_io_service;
    std::unique_ptr<boost::asio::io_service::work> m_ios_work;
    std::atomic<bool>           m_running;
    std::thread                 m_thread;
    EventID                     m_next_event_id;
    std::mutex                  m_mtx;
    std::unique_ptr<SocketPair> m_socket_pair;
};

} // knet
