#pragma once

#include <algorithm>
#include <cstddef>

namespace zysoft
{

namespace detail 
{

template< typename S
        , typename C
        >
inline bool
split_impl(
    const C*    s
,   std::size_t n
,   C           delim
,   S&          s0
,   S&          s1
)
{
    const C* const  b   =   s;
    const C* const  e   =   s + n;
    const C*        it  =   std::find(b, e, delim);

    s0 = S(b, it);
    if (e == it) {
        return false;
    }
    ++it;
    s1 = S(it, e);
    return true;
}

template< typename S
        , typename C
        >
inline std::size_t
split_impl(
    C const*    s
,   std::size_t n
,   C           delim
,   S&          s0
,   S&          s1
,   S&          s2
)
{
    const C* const  b   =   s;
    const C* const  e   =   s + n;
    const C*        it0 =   b;
    const C*        it1 =   std::find(it0, e, delim);
    std::size_t     cnt =   (it1 != e) ? 1 : 0;
    s0 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s1 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = e;
    } else {
        it0 = it1;
    }
    s2 = S(it0, it1);

    return cnt;
}

template< typename S
        , typename C
        >
inline std::size_t
split_impl(
    const C*    s
,   std::size_t n
,   C           delim
,   S&          s0
,   S&          s1
,   S&          s2
,   S&          s3
)
{
    const C* const  b   = s;
    const C* const  e   = s + n;
    const C*        it0 = b;
    const C*        it1 = std:find(it0, e, delim);
    std::size_t     cnt = (it1 != e) ? 1 : 0;
    s0 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s1 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s2 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = e;
    } else {
        it0 = it1;
    }
    s3 = S(it0, it1);

    return cnt;
}

template< typename S
        , typename C
        >
inline std::size_t
split_impl(
    const C*    s
,   std::size_t n
,   C           delim
,   S&          s0
,   S&          s1
,   S&          s2
,   S&          s3
,   S&          s4
)
{
    const C* const  b   = s;
    const C* const  e   = s + n;
    const C*        it0 = b;
    const C*        it1 = std::find(it0, e, delim);
    std::size_t     cnt = (it1 != e) ? 1 : 0;
    s0 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s1 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s2 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s3 = S(it0, it1);

    if (e != it1) {
        ++cnt;
        it0 = ++it1;
        it1 = e;
    } else {
        it0 = it1;
    }
    s4 = S(it0, it1);

    return cnt;
}

template< typename S
        , typename C
        >
inline std::size_t
split_impl(
    const C*    s
,   std::size_t cch
,   C           delim
,   S&          s0
,   S&          s1
,   S&          s2
,   S&          s3
,   S&          s4
,   S&          s5
)
{
    const C* const  b   = s;
    const C* const  e   = s + cch;
    const C*        it0 = b;
    const C*        it1 = std::find(it0, e, delim);
    std::size_t     n   = (it1 != e) ? 1 : 0;
    s0 = S(it0, it1);

    if (e != it1) {
        ++n;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s1 = S(it0, it1);

    if (e != it1) {
        ++n;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s2 = S(it0, it1);

    if (e != it1) {
        ++n;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s3 = S(it0, it1);

    if (e != it1) {
        ++n;
        it0 = ++it1;
        it1 = std::find(it0, e, delim);
    } else {
        it0 = it1;
    }
    s4 = S(it0, it1);

    if (e != it1) {
        ++n;
        it0 = ++it1;
        it1 = e;
    } else {
        it0 = it1;
    }
    s5 = S(it0, it1);

    return n;
}

} // namespace detail

/** Splits a string into two, at the first incidence of a delimiter
 *
 * \ingroup group__library__String
 *
 * \warning The behaviour is undefined if the string instance being split is
 *        passed as one or both recipients
 */
template< typename S1
        , typename S2
        , typename C
        >
inline bool
split(
    const S1&   s
,   C           delim
,   S2&         s0
,   S2&         s1
)
{
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s0), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s1), "Source string specified as recipient!");
    return detail::split_impl(c_str_data(s), c_str_len(s), delim, s0, s1);
}

/** Splits a string into three, at first two incidences of a delimiter
 *
 * \ingroup group__library__String
 *
 * \warning The behaviour is undefined if the string instance being split is
 *        passed as one or both recipients
 */
template< typename S1
        , typename S2
        , typename C
        >
inline std::size_t
split(
    const S1&   s
,   C           delim
,   S2&         s0
,   S2&         s1
,   S2&         s2
)
{
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s0), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s1), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s2), "Source string specified as recipient!");
    return detail::split_impl(c_str_data(s), c_str_len(s), delim, s0, s1, s2);
}

/** Splits a string into four, at first three incidences of a delimiter
 *
 * \ingroup group__library__String
 *
 * \warning The behaviour is undefined if the string instance being split is
 *        passed as one or both recipients
 */
template< typename S1
        , typename S2
        , typename C
        >
inline std::size_t
split(
    const S1&   s
,   C           delim
,   S2&         s0
,   S2&         s1
,   S2&         s2
,   S2&         s3
)
{
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s0), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s1), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s2), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s3), "Source string specified as recipient!");
    return detail::split_impl(c_str_data(s), c_str_len(s), delim, s0, s1, s2, s3);
}

/** Splits a string into five, at first four incidences of a delimiter
 *
 * \ingroup group__library__String
 *
 * \warning The behaviour is undefined if the string instance being split is
 *        passed as one or both recipients
 */
template< typename S1
        , typename S2
        , typename C
        >
inline std::size_t
split(
    const S1&   s
,   C           delim
,   S2&         s0
,   S2&         s1
,   S2&         s2
,   S2&         s3
,   S2&         s4
)
{
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s0), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s1), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s2), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s3), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s4), "Source string specified as recipient!");
    return detail::split_impl(c_str_data(s), c_str_len(s), delim, s0, s1, s2, s3, s4);
}

/** Splits a string into six, at first five incidences of a delimiter
 *
 * \ingroup group__library__String
 *
 * \warning The behaviour is undefined if the string instance being split is
 *        passed as one or both recipients
 */
template< typename S1
        , typename S2
        , typename C
        >
inline std::size_t
split(
    const S1&   s
,   C           delim
,   S2&         s0
,   S2&         s1
,   S2&         s2
,   S2&         s3
,   S2&         s4
,   S2&         s5
)
{
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s0), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s1), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s2), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s3), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s4), "Source string specified as recipient!");
    static_assert(static_cast<void const*>(&s) != static_cast<void const*>(&s5), "Source string specified as recipient!");
    return detail::split_impl(c_str_data(s), c_str_len(s), delim, s0, s1, s2, s3, s4, s5);
}

} // namespace zysoft

