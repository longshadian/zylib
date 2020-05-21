#pragma once

#include <climits>
#include <cerrno>
#include <limits>
#include <charconv>
#include <system_error>
#include <iostream>
#include <string_view>

namespace zysoft
{

inline void throw_error(const std::error_code& ec)
{
    if (ec)
        throw std::runtime_error(ec.message());
}

template <bool S>
struct skip_blank
{
    enum {value = S};
};

template< typename V
        , typename SS = skip_blank<true>
        , typename C = char
        >
struct convert
{
    typedef V value_type;
    typedef typename std::char_traits<C>::char_type char_type;
    typedef SS skip_space_type;

    static value_type to_number(std::string_view sv, std::error_code& ec)
    {
        return to_number(sv.data(), sv.data() + sv.length(), ec);
    }

    static value_type to_number(const char_type* first, const char_type* last, std::error_code& ec)
    {
        return cvt_impl(first, last, &ec);
    }

    static value_type to_number(std::string_view sv)
    {
        return to_number(sv.data(), sv.data() + sv.length());
    }

    static value_type to_number(const char_type* first, const char_type* last)
    {
        std::error_code ec{};
        value_type v = to_number(first, last, ec);
        throw_error(ec);
        return v;
    }

    static value_type to_number_default(std::string_view sv, value_type d)
    {
        return to_number_default(sv.data(), sv.data() + sv.length(), d);
    }

    static value_type to_number_default(const char_type* first, const char_type* last, value_type d)
    {
        return cvt_impl(first, last, nullptr, &d);
    }

private:
    static value_type cvt_impl(const char_type* first, const char_type* last, std::error_code* ec = nullptr, const value_type* d = nullptr)
    {
        if constexpr (skip_space_type::value != 0) {
            while (first != last && std::isspace(*first)) {
                ++first;
            }
        }

        value_type v{};
        std::from_chars_result ret = std::from_chars(first, last, v);
        if (ret.ec == std::errc::invalid_argument) {
            if (ec)
                *ec = std::make_error_code(ret.ec);
            return d ? *d : value_type{};
        }
        if (ret.ec == std::errc::result_out_of_range) {
            if (ec)
                *ec = std::make_error_code(ret.ec);
            return d ? *d : value_type{};
        }
        return v;
    }

};


}

