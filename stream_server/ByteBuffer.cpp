#include "ByteBuffer.h"

#include <sstream>
#include <iostream>

namespace network {

ByteBufferOverflowException::ByteBufferOverflowException(size_t pos, size_t size, size_t remian_size)
    : ByteBufferException(toString(pos, size, remian_size))
{
}

std::string ByteBufferOverflowException::toString(size_t pos, size_t size, size_t remian_size)
{
    std::ostringstream ss;
    ss << "overflow: pos:" << pos << " size:" << size << " "
        << " in ByteBuffer remian size:" << remian_size;
    return ss.str();
}

ByteBuffer::ByteBuffer()
    : ByteBuffer(DEFAULT_SIZE)
{
}

ByteBuffer::ByteBuffer(size_t res)
    : m_rpos(0), m_wpos(0), m_storage()
{
    m_storage.reserve(res);
}

ByteBuffer::ByteBuffer(std::vector<uint8_t> buf)
    : m_rpos(0), m_wpos(0), m_storage(std::move(buf))
{
    m_wpos = m_storage.size();
}

ByteBuffer::ByteBuffer(const void* src, size_t len) 
    : m_rpos(0), m_wpos(0), m_storage(static_cast<const uint8_t*>(src), static_cast<const uint8_t*>(src) + len) 
{
    m_wpos = m_storage.size();
}

ByteBuffer::~ByteBuffer() 
{
}

ByteBuffer::ByteBuffer(ByteBuffer&& buf)
    : m_rpos(buf.m_rpos), m_wpos(buf.m_wpos), m_storage(std::move(buf.m_storage))
{
}

ByteBuffer::ByteBuffer(const ByteBuffer& right)
    : m_rpos(right.m_rpos), m_wpos(right.m_wpos), m_storage(right.m_storage)
{
}

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& right)
{
    if (this != &right) {
        m_rpos = right.m_rpos;
        m_wpos = right.m_wpos;
        m_storage = right.m_storage;
    }
    return *this;
}

ByteBuffer& ByteBuffer::operator=(ByteBuffer&& right) 
{
    if (this != &right) {
        m_rpos = right.m_rpos;
        m_wpos = right.m_wpos;
        m_storage = std::move(right.m_storage);
    }
    return *this;
}

size_t ByteBuffer::byteSize() const
{
    if (m_rpos > m_wpos)
        throw ByteBufferException("byteSize read overflow");
    return m_wpos - m_rpos;
}

bool ByteBuffer::byteEmpty() const
{
    return m_rpos == m_storage.size();
}

void ByteBuffer::clear()
{
    m_rpos = 0;
    m_wpos = 0;
    m_storage.clear();
}

void ByteBuffer::resize(size_t s)
{
    m_storage.resize(s, 0);
    m_rpos = 0;
    m_wpos = m_storage.size();
}

void ByteBuffer::reserve(size_t s)
{
    if (s > m_storage.size())
        m_storage.reserve(s);
}

const uint8_t* ByteBuffer::data() const
{
    if (byteEmpty())
       throw ByteBufferException("buf is empty!");
    return &m_storage[m_rpos];
}

void ByteBuffer::readSkip(size_t skip)
{
    if (skip > byteSize())
        throw ByteBufferOverflowException(m_rpos, skip, byteSize());
    m_rpos += skip;
}

void ByteBuffer::shrinkToFit()
{
    if (m_rpos == 0)
        return;
    auto len = byteSize();
    if (len == 0) {
        m_rpos = 0;
        m_wpos = 0;
        //m_storage.shrink_to_fit();
        m_storage.clear();
        return;
    }

    /*
    std::ostringstream ostm{};
    ostm << "befor rpos " << m_rpos
        << " wpos " << m_wpos
        << " size " << m_storage.capacity();
    */

    std::memmove(m_storage.data(), &m_storage[m_rpos], len);
    m_rpos = 0;
    m_wpos = len;
    m_storage.resize(m_wpos);
    //m_storage.shrink_to_fit();

    /*
    ostm << " after rpos " << m_rpos
        << " wpos " << m_wpos
        << " size " << m_storage.capacity();
    std::cout << ostm.str() << "\t";
    */
}

} // network
