#ifndef _MYSQLCPP_UTILS_H
#define _MYSQLCPP_UTILS_H

#include <memory>

namespace mysqlcpp {

namespace util {

template <typename T, typename... Args>
inline
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
}

}
}

#endif
