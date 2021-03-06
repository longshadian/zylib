#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>


#include <zysoft/zysoft.h>
#include <zysoft/memory/util/allocator_selector.h>

namespace zysoft
{

// TODO 

template< typename T
        , std::size_t N = 256
        , typename A = typename allocator_selector<T>::allocator_type
        >
class auto_buffer
    : protected A
{
    typedef typename allocator_selector<A>::allocator_traits_type alloc_traits;

    enum {space = N};
public:
    typedef auto_buffer<T, N, A>                    class_type;
    typedef typename alloc_traits::allocator_type   allocator_type;
    typedef typename alloc_traits::value_type       value_type;
    typedef typename alloc_traits::size_type        size_type;
    typedef typename alloc_traits::pointer          pointer;
    typedef typename alloc_traits::const_pointer    const_pointer;

    typedef T&                                      reference;
    typedef const T&                                const_reference;
    typedef T*                                      iterator;
    typedef const iterator                          const_iterator;
    typedef std::reverse_iterator<T>                reverse_iterator;
    typedef const reverse_iterator                  const_reverse_iterator;

    static_assert(std::is_pod_v<T>, "auto_buffer T must be POD!");
public:

    explicit    auto_buffer(size_type n);
                ~auto_buffer();

                auto_buffer(const class_type&) = delete;
    class_type& operator=(const class_type&) = delete;

public:
    bool resize(size_type n)
    {
        if (n == 0) {
            if (external_buffer()) {
                dealloc(m_buffer, m_items);
                m_external = false;
                m_buffer = &m_internal[0];
            }
            m_items = n;
            return true;
        }

        if (m_items < n) {
            if (external_buffer()) {
                pointer new_buffer = realloc(m_buffer, m_items, n);
                if (!new_buffer)
                    return false;
                m_buffer = new_buffer;
            } else {
                if (space < n) {
                    pointer new_buffer = alloc(n);
                    if (!new_buffer)
                        return false;
                    block_copy(new_buffer, m_buffer, m_items);
                    m_buffer = new_buffer;
                    m_external = true;
                } else {
                    // Nothing to do
                }
            }
        } else {
            if (external_buffer()) {
                // Nothing to do
            } else {
                // Nothing to do
            }
        }
        m_items = n;
        return true;
    }

    void swap(class_type& rhs)
    {
        std::swap(this->m_internal, rhs.m_internal);
        std::swap(this->m_buffer, rhs.m_buffer);
        std::swap(this->m_items, rhs.m_items);
        std::swap(this->m_external, rhs.m_external);
    }

    bool empty() const
    {
        return m_items == 0;
    }

    size_type size() const
    {
        return m_items;
    }

    static size_type internal_size()
    {
        return space;
    }

    reference operator[](size_type index)
    {
        return m_buffer[index];
    }

    const_reference operator[](size_type index) const
    {
        return m_buffer[index];
    }

    pointer data()
    {
        return m_buffer;
    }

    const_pointer data() const
    {
        return m_buffer;
    }

    iterator begin()
    {
        return m_buffer;
    }

    iterator end()
    {
        return m_buffer + m_items;
    }

    const_iterator begin() const
    {
        return m_buffer;
    }

    const_iterator end() const
    {
        return m_buffer + m_items;
    }

    const_iterator cbegin() const
    {
        return m_buffer + m_items;
    }

    const_iterator cend() const
    {
        return m_buffer + m_items;
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

private:
    value_type              m_internal[N];
    pointer                 m_buffer;
    size_type               m_items;
    bool                    m_external;

private:
    pointer alloc(size_type n)
    {
        return alloc_traits::allocate(get_allocator(), n);
        //return get_allocator().allocate(n);
    }

    void dealloc(pointer p, size_type n)
    {
        return alloc_traits::deallocate(get_allocator(), p, n);
        //get_allocator().deallocate(p, n);
    }

    pointer realloc(pointer current_p, size_type current_n, size_type new_n)
    {
        pointer new_p = alloc(new_n);
        if (!new_p)
            return new_p;
        block_copy(new_p, current_p, current_n);
        dealloc(current_p, current_n);
        return new_p;
    }

    allocator_type& get_allocator()
    {
        return *this;
    }

    bool external_buffer() const
    {
        return m_external;
    }

    static void block_copy(void* s, const void* d, size_type len)
    {
        std::memcpy(s, d, len * sizeof(T));
    }

    static void block_fill(void* s, size_type len)
    {
        std::memset(s, 0, len * sizeof(T));
    }
};


template< typename T
        , std::size_t N
        , typename A
        >
inline auto_buffer<T, N, A>::auto_buffer(size_type n)
    : m_internal()
    , m_buffer(n < N ? &m_internal[0] : alloc(n))
    , m_items(m_buffer ? n : 0)
    , m_external(n > N)
{
    block_fill(m_buffer, m_items);
}

template< typename T
        , std::size_t N
        , typename A
        >
inline auto_buffer<T, N, A>::~auto_buffer()
{
    if (external_buffer()) {
        dealloc(m_buffer, m_items);
    }
}

} // namespace zysoft

