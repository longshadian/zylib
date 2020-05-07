#pragma once

#include <cctype>
#include <algorithm>

namespace zysoft
{

/** Converts all characters in the string to upper case.
 *
 * \ingroup group__library__String
 */
template <typename S>
inline S& make_upper(S& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](int c) { return static_cast<char>(::toupper(c)); });
    return s;
}

/** Converts all characters in the string to lower case.
 *
 * \ingroup group__library__String
 */
template <typename S>
inline S& make_lower(S& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](int c) { return static_cast<char>(::tolower(c)); });
    return s;
}

/** Returns a copy of the source string in which all characters have
 *   been converted to upper case.
 *
 * \ingroup group__library__String
 */
template <typename S>
inline S to_upper(const S& s)
{
    S r(s);
    make_upper(r);
    return r;
}

/** Returns a copy of the source string in which all characters have
 *   been converted to lower case.
 *
 * \ingroup group__library__String
 */
template <typename S>
inline S to_lower(const S& s)
{
    S r(s);
    make_lower(r);
    return r;
}

} // namespace zysoft

