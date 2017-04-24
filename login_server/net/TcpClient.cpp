#include "TcpClient.h"

TcpClient::TcpClient(boost::asio::io_service& io_service) 
    : m_io_service(io_service)
    , m_socket(io_service)
    , m_write_msgs()
{
}

TcpClient::~TcpClient()
{
    closeSocket();
}

void TcpClient::write(const Message& msg)
{
    m_io_service.post([this, msg]
        {
            bool wait_write = !m_write_msgs.empty();
            m_write_msgs.push_back(std::move(msg));
            if (!wait_write) {
                doWrite();
            }
        });
}

void TcpClient::shutdown()
{
    m_io_service.post([this]() 
        { 
            closeSocket();
        });
}

void TcpClient::setConnectionCB(ConnectionCB cb)
{
    m_connection_cb = std::move(cb);
}

void TcpClient::setIOErrorCB(IOErrorCB cb)
{
    m_io_error_cb = std::move(cb);
}

void TcpClient::connect(boost::asio::ip::tcp::resolver::iterator it)
{
    boost::asio::async_connect(m_socket, it,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
            if (m_connection_cb)
                m_connection_cb(ec);
            if (!ec) {
                doReadHeader();
            }
        });
}

void TcpClient::doReadHeader()
{
    boost::asio::async_read(m_socket,
        boost::asio::buffer(m_read_head.data(), m_read_head.size()),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (ec) {
                    m_io_error_cb(ec);
                    return;
                }
                std::memcpy(&m_total, m_read_head.data(), 4);
                std::memcpy(&m_msg_id, m_read_head.data() + 4, 4);
                if (m_total <= 8) {
                    closeSocket();
                    return;
                }

                m_read_body.resize(m_total - 4 - 4);
                doReadBody();
            });
}

void TcpClient::doReadBody()
{
    boost::asio::async_read(m_socket,
        boost::asio::buffer(m_read_body),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (ec) {
                    m_io_error_cb(ec);
                    closeSocket();
                    return;
                }
                m_read_head.fill(0);
                m_read_body.clear();
                m_total = 0;
                m_msg_id = 0;
                doReadHeader();
            });
}

void TcpClient::doWrite()
{
    boost::asio::async_write(m_socket,
        boost::asio::buffer(m_write_msgs.front().data(),
          m_write_msgs.front().size()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (ec) {
                m_io_error_cb(ec);
                closeSocket();
                return;
            }

            m_write_msgs.pop_front();
            if (!m_write_msgs.empty()) {
                doWrite();
            }
        });
}

void TcpClient::closeSocket()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

}
