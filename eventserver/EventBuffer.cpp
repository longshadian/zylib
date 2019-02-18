#include "EventBuffer.h"

#include <cstring>
#include <event2/buffer.h>
#include <event2/buffer_compat.h>

EventBuffer::EventBuffer()
{
    m_buffer = evbuffer_new();
}

EventBuffer::~EventBuffer()
{
    evbuffer_free(m_buffer);
}

bool EventBuffer::append(const void * buffer, size_t len)
{
    return evbuffer_add(m_buffer, buffer, (int)len);
}

bool EventBuffer::append(const void * buffer, int len)
{
	return evbuffer_add(m_buffer, buffer, len) == 0;
}


bool EventBuffer::append(const void * buffer)
{
    auto len = strlen((const char*)buffer);
    return append(buffer, len);
}

bool EventBuffer::append(EventBuffer * buffer)
{
    return evbuffer_add_buffer(m_buffer, buffer->m_buffer) == 0;
}

bool EventBuffer::erase(int len)
{
    return evbuffer_drain(m_buffer, len) == 0;
}

void EventBuffer::clear()
{
    erase(getSize());
}

int EventBuffer::getSize() const
{
    return (int)evbuffer_get_length(m_buffer);
}

int EventBuffer::take(void * buffer, int len)
{
    return evbuffer_remove(m_buffer, buffer, len);
}

int EventBuffer::get(void* buffer, int len)
{
    return int(evbuffer_copyout(m_buffer, buffer, static_cast<size_t>(len)));
}

int EventBuffer::read(int fd)
{
    return evbuffer_read(m_buffer, fd, -1);
}

int EventBuffer::write(int fd)
{
	return evbuffer_write(m_buffer, fd);
}