#pragma once

#include <string>
#include <string_view>

namespace zysoft
{
namespace web 
{
namespace detail
{

enum class token_type
{
    Unknown = 0,
    Letter = 1,
    Number = 2,
    Identifier = 4,		// name
    Puncatuation = 5,	// punctuation
    Keyword = 6,        // keyword
    Eof = 7,            // end of file or stream 
};

template <typename T>
struct basic_token
{
    basic_token() {}
    ~basic_token() {}

    int         index_;
    token_type  type_;
    T           value_;
};

template <typename T>
struct lexer
{
public:
    typedef basic_token<T>          token_type;
    typedef typename T::value_type  value_type;
    typedef typename T::const_iterator const_iterator;
public:

    lexer() {}
    ~lexer() {}

    int read_token(token_type* t)
    {

        return 0;
    }

private:
    int read_letter(token_type* t)
    {
        const_iterator p = current_p_;
        while (current_p_) {
            if (value_type('a') <= *current_p_ && *current_p_ <= value_type('z')) {
                ++current_p_;
            } else {
                break;
            }
        }
        if (p == current_p_)
            break;
    }

    const T             src_;
    const_iterator      current_p_;
    token_type          token_;
};


} // namespace detail
} // namespace web
} // namespace zysoft
