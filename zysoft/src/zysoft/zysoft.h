#pragma once

#include <cassert>

#define ZYSOFT_ASSERT(expression) assert(expression)
#define ZYSOFT_MESSAGE_ASSERT(expression, m) assert(expression)

#define ZYSOFT_ALLOCATOR_SELECTOR_USE_STD_ALLOCATOR

#define ZYSOFT_NODISCARD            [[nodiscard]]
#define ZYSOFT_MAYBE_UNUSED         [[maybe_unused]]
#define ZYSOFT_NOEXCEPT             noexcept

namespace zysoft 
{

} // namespace zysoft


