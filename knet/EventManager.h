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

struct EventMsg
{
    EventID m_event_id;
    enum  { Size = sizeof(EventMsg) };

    std::vector<uint8_t> serializeToBinary() const
    {
        std::vector<uint8_t> buf{};
        buf.resize(Size);
        std::memcpy(buf.data(), &m_event_id, buf.size());
        return buf;
    }
};
static_assert(std::is_pod<EventMsg>::type, "EventMsg mush be TOD!");

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
    void AsyncWrite(const EventMsg& msg);

private:
    void DoWrite();

private:
    boost::asio::io_service& m_io_service;
    boost::asio::local::stream_protocol::socket m_sock_read;
    boost::asio::local::stream_protocol::socket m_sock_write;

    std::array<uint8_t, EventMsg::Size>     m_read_buf;
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

private:
    void ThreadRun();

private:
    boost::asio::io_service     m_io_service;
    std::unique_ptr<boost::asio::io_service::work> m_ios_work;
    std::atomic<bool>           m_running;
    std::thread                 m_thread;
    std::mutex                  m_mtx;
    std::unique_ptr<SocketPair> m_socket_pair;
};

} // knet
