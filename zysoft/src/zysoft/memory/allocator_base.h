#pragma once

#include <cstddef>
#include <limits>
#include <exception>


namespace zysoft
{

template< typename T
        , typename A
        >
struct allocator_base
{
    typedef allocator_base<T, A>        class_type;
    typedef T                           value_type;
    typedef std::ptrdiff_t              difference_type;
    typedef std::size_t                 size_type;
    typedef value_type*                 pointer;
    typedef const value_type*           const_pointer;
    typedef value_type&                 reference_type;
    typedef const value_type&           const_reference_type;

private:
    typedef A                           allocator_type;

public:
    ZYSOFT_NODISCARD pointer allocate(size_type n)
    {
        if (n > std::numeric_limits<size_type>::max() / sizeof(value_type))
            throw std::bad_alloc();
        allocator_type* alloc = reinterpret_cast<allocator_type*>(this);
        if (auto p = alloc->allocate_impl(n * sizeof(value_type)))
            return p;
        throw std::bad_alloc();
    }

    void deallocate(pointer p, size_type n) noexcept
    {
        reinterpret_cast<allocator_type*>(this)->do_deallocate(p, n);
    }
};

} // namespace zysoft


