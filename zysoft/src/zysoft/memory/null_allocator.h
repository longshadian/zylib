#pragma once

#include <zysoft/zysoft.h>
#include <zysoft/memory/allocator_base.h>

namespace zysoft
{

template <typename T>
struct null_allocator
    : public allocator_base<T, null_allocator<T>>
{
private:
    typedef allocator_base<T, null_allocator<T>> parent_class_type;

public:
    typedef null_allocator<T>                               class_type;
    typedef typename parent_class_type::difference_type     difference_type;
    typedef typename parent_class_type::size_type           size_type;    
    typedef typename parent_class_type::pointer             pointer;
    typedef typename parent_class_type::const_pointer       const_pointer;
    typedef typename parent_class_type::reference_type      reference_type;
    typedef typename parent_class_type::const_reference_type const_reference_type;

public:
    null_allocator() = default;

    template <typename U>
    null_allocator(const U&)
    {
    }

    ~null_allocator() = default;

private:
    friend parent_class_type;
    pointer allocate_impl(ZYSOFT_MAYBE_UNUSED size_type n)
    {
        return nullptr;
    }

    void deallocate_impl(ZYSOFT_MAYBE_UNUSED pointer p, ZYSOFT_MAYBE_UNUSED size_type n)
    {
    }
};


} // namespace zysoft


