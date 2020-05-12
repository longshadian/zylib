#pragma once

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
    Identifier = 4,        // name
    Puncatuation = 5,    // punctuation
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

    bool read_token(token_type* t)
    {

        return 0;
    }

    bool eof() const
    {
        return p0_ == src_.end();
    }

private:
    int read_letter(token_type* t)
    {
        const_iterator pb = p0_;
        while (!eof()) {
            if ((value_type('a') <= *p0_ && *p0_ <= value_type('z'))
                || (value_type('A') <= *p0_ && * p0_ <= value_type('Z'))) {
                ++p0_;
            } else {
                break;
            }
        }

        return pb != p0_;
    }

    int read_number(token_type* t)
    {
        const_iterator pb = p0_;
        while (!eof()) {
            if ((value_type('a') <= *p0_ && *p0_ <= value_type('z'))
                || (value_type('A') <= *p0_ && * p0_ <= value_type('Z'))) {
                ++p0_;
            } else {
                break;
            }
        }

        return pb != p0_;
    }

    const T             src_;
    const_iterator      p0_;
    token_type          token_;
};





} // namespace detail

} // namespace web

} // namespace zysoft

