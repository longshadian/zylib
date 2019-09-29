#include "network/TcpSocket.h"

namespace network
{

TcpSocket::TcpSocket(IOContextPtr ioc)
    : m_ioc(ioc)
    , m_socket(ioc->m_ioctx)
{
}

TcpSocket::~TcpSocket()
{
}

} // namespace tcpsocket
