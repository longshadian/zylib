#pragma once

#include <string>
#include <sstream>

namespace zylib {
namespace detail {

inline
std::string concatDetail(std::ostringstream* ostm)
{
    return ostm->str();
}

template <typename T, typename... Args>
inline
std::string concatDetail(std::ostringstream* ostm, T&& t, Args&&... arg)
{
    (*ostm) << std::forward<T>(t);
    return concatDetail(ostm, std::forward<Args>(arg)...);
}

} /// detail


template <typename... Args>
inline
std::string concat(Args&&... arg)
{
    std::ostringstream ostm;
    return detail::concatDetail(&ostm, std::forward<Args&&>(arg)...);
}

} /// zylib
