#pragma once

#include <cstddef>
#include <memory>

namespace zysoft
{

template< typename T
        , std::size_t N = 256
        , typename A = typename std::allocator<T>::type_info
        >
class auto_buffer
    : public A
{
public:
    typedef auto_buffer<T, N, A> class_type;
    typedef T                   value_type;
    typedef std::size_t         size_type;
    typedef T&                  reference_type;
    typedef const T&            const_reference_type;
    typedef T*                  pointer_type;
    typedef const T*            const_pointer_type;
    typedef T*                  iterator;
    typedef const iterator      const_iterator;
    typedef std::reverse_iterator<T> reverse_iterator;
    typedef std::const_reverse_iterator<T> const_reverse_iterator;

public:
    explicit                    auto_buffer(size_type n);
                                ~auto_buffer();

public:
    void                        resize(size_type n);
    void                        swap();
    bool                        empty() const;
    size_type                   size() const;
    static size_type            internal_size() const;

    reference_type              operator[](size_type index);
    constreference_type         operator[](size_type index) const;
    pointer_type                data();
    const_pointer_type          data() const;

    iterator                    begin();
    iterator                    end();
    iterator                    begin() const;
    iterator                    end() const;
    const_iterator              cbegin() const;
    const_iterator              cend() const;

    reverse_iterator            rbegin();
    reverse_iterator            rend();
    const_reverse_iterator      rbegin() const;
    const_reverse_iterator      rend() const;

private:
    pointer_type                m_buffer;
    size_type                   m_items;
    value_type                  m_internal[N];

private:
                                auto_buffer(const class_type&) = delete;
    class_type&                 operator=(const class_type&) = delete;
};


} // namespace zysoft

