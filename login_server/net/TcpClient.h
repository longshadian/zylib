#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <functional>

#include <boost/asio.hpp>

class Message
{
public:
    using StorageType = std::vector<uint8_t>;
public:
    explicit Message(const std::string& data) 
        : Message(data.c_str(), data.length()) 
    {
    }

    explicit Message(const void* data, size_t len)
        : m_data()
    {
        m_data.resize(len);
        std::memcpy(m_data.data(), data, len);
    }

    explicit Message(StorageType data) 
        : m_data(std::move(data)) 
    {
    }

    ~Message() = default;

    Message(const Message& rhs)
        : m_data(rhs.m_data)
    {
    }

    Message& operator=(const Message& rhs)
    {
        if (this != &rhs) {
            m_data = rhs.m_data;
        }
        return *this;
    }

    Message(Message&& rhs)
        : m_data(std::move(rhs.m_data))
    {
    }

    Message& operator=(Message&& rhs)
    {
        if (this != &rhs) {
            m_data = std::move(rhs.m_data);
        }
        return *this;
    }

public:
    const void* data() const { return m_data.data(); }
    size_t size() const { return m_data.size(); }
private:
    StorageType m_data;
};

class TcpClient
{
public:
    using ConnectionCB = std::function<void(const boost::system::error_code&)>;
    using IOErrorCB  = std::function<void(const boost::system::error_code&)>;
public:
    TcpClient(boost::asio::io_service& io_service);
    ~TcpClient();

    void connect(boost::asio::ip::tcp::resolver::iterator it);
    void write(const Message& msg);
    void shutdown();

    void setConnectionCB(ConnectionCB cb);
    void setIOErrorCB(IOErrorCB cb);
private:
    void doReadHeader();
    void doReadBody();
    void doWrite();
    void closeSocket();
private:
    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::socket m_socket;

    ConnectionCB m_connection_cb;
    IOErrorCB m_io_error_cb;

    std::deque<Message> m_write_msgs;

    std::array<uint8_t, 4+4> m_read_head;
    std::vector<uint8_t>     m_read_body;
    int32_t                  m_total;
    int32_t                  m_msg_id;
};

}

