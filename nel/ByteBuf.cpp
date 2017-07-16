#include "ByteBuf.h"

#include <sstream>
#include <iostream>

namespace nlnet {

ByteBufOverflowException::ByteBufOverflowException(size_t pos, size_t size, size_t remian_size)
    : ByteBufException(toString(pos, size, remian_size))
{
}

std::string ByteBufOverflowException::toString(size_t pos, size_t size, size_t remian_size)
{
    std::ostringstream ss;
    ss << "overflow: pos:" << pos << " size:" << size << " "
        << " in ByteBuffer remian size:" << remian_size;
    return ss.str();
}

ByteBuf::ByteBuf()
    : ByteBuf(DEFAULT_SIZE)
{
}

ByteBuf::ByteBuf(size_t res)
    : m_rpos(0), m_wpos(0), m_storage()
{
    m_storage.reserve(res);
}

ByteBuf::ByteBuf(std::vector<uint8_t> buf)
    : m_rpos(0), m_wpos(0), m_storage(std::move(buf))
{
    m_wpos = m_storage.size();
}

ByteBuf::ByteBuf(const void* src, size_t len) 
    : m_rpos(0), m_wpos(0), m_storage(static_cast<const uint8_t*>(src), static_cast<const uint8_t*>(src) + len) 
{
    m_wpos = m_storage.size();
}

ByteBuf::~ByteBuf() 
{
}

ByteBuf::ByteBuf(ByteBuf&& buf)
    : m_rpos(buf.m_rpos), m_wpos(buf.m_wpos), m_storage(std::move(buf.m_storage))
{
}

ByteBuf::ByteBuf(const ByteBuf& right)
    : m_rpos(right.m_rpos), m_wpos(right.m_wpos), m_storage(right.m_storage)
{
}

ByteBuf& ByteBuf::operator=(const ByteBuf& right)
{
    if (this != &right) {
        m_rpos = right.m_rpos;
        m_wpos = right.m_wpos;
        m_storage = right.m_storage;
    }
    return *this;
}

ByteBuf& ByteBuf::operator=(ByteBuf&& right) 
{
    if (this != &right) {
        m_rpos = right.m_rpos;
        m_wpos = right.m_wpos;
        m_storage = std::move(right.m_storage);
    }
    return *this;
}

size_t ByteBuf::byteSize() const
{
    if (m_rpos > m_wpos)
        throw ByteBufException("byteSize read overflow");
    return m_wpos - m_rpos;
}

bool ByteBuf::byteEmpty() const
{
    return m_rpos == m_storage.size();
}

void ByteBuf::clear()
{
    m_rpos = 0;
    m_wpos = 0;
    m_storage.clear();
}

void ByteBuf::resize(size_t s)
{
    m_storage.resize(s, 0);
    m_rpos = 0;
    m_wpos = m_storage.size();
}

void ByteBuf::reserve(size_t s)
{
    if (s > m_storage.size())
        m_storage.reserve(s);
}

const uint8_t* ByteBuf::data() const
{
    if (byteEmpty())
       throw ByteBufException("buf is empty!");
    return &m_storage[m_rpos];
}

void ByteBuf::readSkip(size_t skip)
{
    if (skip > byteSize())
        throw ByteBufOverflowException(m_rpos, skip, byteSize());
    m_rpos += skip;
}

void ByteBuf::shrinkToFit()
{
    if (m_rpos == 0)
        return;
    auto len = byteSize();
    if (len == 0) {
        m_rpos = 0;
        m_wpos = 0;
        m_storage.shrink_to_fit();
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

} // nlnet
