#ifndef _EVENT_BUFFER_H_
#define _EVENT_BUFFER_H_

#include <cstddef>

struct evbuffer;

class EventBuffer
{
public:
                            EventBuffer();
                            ~EventBuffer();

    bool                    append(const void * buffer, size_t len);
    bool                    append(const void * buffer, int len);
    bool                    append(const void * buffer);
    bool                    append(EventBuffer * buffer);
    bool                    erase(int len);
    void                    clear();
    int                     getSize() const;
    int                     take(void * buffer, int len);
    int                     get(void* buffer, int len);
    int                     read(int fd);
    int                     write(int fd);
private:
    struct evbuffer *       m_buffer;
};

#endif