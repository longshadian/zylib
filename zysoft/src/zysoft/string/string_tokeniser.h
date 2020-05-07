#pragma once

# include <iterator>                     // for std::distance()

namespace zysoft
{

#if 0

template <int V_shouldSkip>
struct skip_discriminator_type_
{};

template <>
struct skip_discriminator_type_<0>
{};



// string_tokeniser_type_traits
/** A traits class for detecting features of the string type and the value type used to specialise string_tokeniser
 *
 * \ingroup group__library__String
 *
 * This traits class has three responsibilities. First, it defines a number of
 * member types that are used by the string_tokeniser and
 * string_tokeniser::iterator classes: value_type, const_iterator_type.
 *
 * Second, it provides a means by which the iterators of the string_tokeniser
 * specialisation's string type can be elicited, by defining the (static)
 * methods begin() and end().
 *
 * Third, it provides a means by which an instance of the string_tokeniser
 * specialisation's value type can be constructed from a pair of iterator of
 * the string type, by defining the (static) create() method.
 *
 * \param S The string tokeniser string type
 * \param V The string tokeniser value type
 */
template< typename S
        , typename V
        >
struct string_tokeniser_type_traits
{
/// \name Member Types
/// @{
private:
    /// The string type
    typedef S                                       string_type;
    /// The tokeniser value type
    typedef V                                       tokeniser_value_type;

public:
    /// The value type
    typedef typename S::value_type                  value_type;
    /// The non-mutable (const) iterator type
    typedef typename S::const_iterator              const_iterator_type;
/// @}

/// \name Operations
/// @{
public:
    /// Returns the start of the contained sequence of the given string
    static const_iterator_type begin(string_type const& s)
    {
        return s.begin();
    }

    /// Returns the end of the contained sequence of the given string
    static const_iterator_type end(string_type const& s)
    {
        return s.end();
    }

    /// Creates an instance of the string from the given range [f:t)
    static tokeniser_value_type create(const_iterator_type f, const_iterator_type t)
    {
        /* There's a bug in the Metrowerks 3.0 standard library string
         * implementation, such that constructing from a range leaves
         * a corrupted sequence due to an optimistic allocation
         * requirement calculation result being stored as the length
         */
#if defined(STLSOFT_COMPILER_IS_MWERKS) || \
    (   (   defined(STLSOFT_COMPILER_IS_INTEL) || \
            defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER == 1300))

        /// The size type
        typedef typename S::size_type         size_type;

        return tokeniser_value_type(&*f, static_cast<size_type>(t - f));
#else /* ? compiler */
        return tokeniser_value_type(f, t);
#endif /* compiler */
    }
/// @}
};

// string_tokeniser_comparator
/** A generic comparator, used to specialise string_tokeniser, that covers most string and delimiter types
 *
 * \ingroup group__library__String
 *
 * This the default tokeniser comparator, providing functionality for both
 * single-character and fixed string delimiters.
 *
 * \param D The delimiter type
 * \param S The string type
 * \param T The traits type
 */
template< typename D
        , typename S
        , typename T
        >
struct string_tokeniser_comparator
{
/// \name Member Types
/// @{
public:
    /// The delimiter type
    typedef D                                                   delimiter_type;
    /// The string type
    typedef S                                                   string_type;
    /// The traits type
    typedef T                                                   traits_type;
    /// The non-mutating (const) iterator type
    typedef typename traits_type::const_iterator_type           const_iterator;
private:
    typedef string_tokeniser_comparator<D, S, T>                class_type;
/// @}

/// \name Implementation
/// @{
private:
#if defined(STLSOFT_CF_MEMBER_TEMPLATE_FUNCTION_SUPPORT) && \
    (   !defined(STLSOFT_COMPILER_IS_MSVC) || \
        _MSC_VER >= 1200)
    /// Evaluates whether the contents of the two sequences are equivalent to the given extent
    template<   ss_typename_param_k I1
            ,   ss_typename_param_k I2
            >
    static bool is_equal_(I1 p1, I2 p2, ss_size_t n)
    {
        for(; n-- > 0; ++p1, ++p2)
        {
            if(*p1 != *p2)
            {
                return false;
            }
        }

        return true;
    }

    /// Evaluates whether the delimiter and the sequence are equivalent to the extent of the delimiter
    template<   ss_typename_param_k D1
            ,   ss_typename_param_k I
            >
    static bool is_equal_(D1 const& delim, I &p2)
    {
        return class_type::is_equal_(delim.begin(), p2, delim.length());
    }

    /// Returns the length of the delimiter
    template <ss_typename_param_k D1>
    static ss_size_t get_length_(D1 const& delim)
    {
        return delim.length();
    }
#else /* ? STLSOFT_CF_MEMBER_TEMPLATE_FUNCTION_SUPPORT */
    /// Evaluates whether the contents of the two sequences are equivalent to the given extent
    static bool is_equal_(string_type const& lhs, typename string_type::value_type const* rhs)
    {
        { for(ss_size_t i = 0, n = lhs.length(); i < n; ++i)
        {
            if(lhs[i] != rhs[i])
            {
                return false;
            }
        }}

        return true;
    }

    /// Returns the length of the delimiter
    static ss_size_t get_length_(string_type const& s)
    {
        return s.length();
    }
#endif // STLSOFT_CF_MEMBER_TEMPLATE_FUNCTION_SUPPORT

    /// Evaluates whether the delimiter and the sequence are equivalent to the extent of the delimiter
    static bool is_equal_(ss_char_a_t const delim, const_iterator &it)
    {
        return delim == *it;
    }
    /// Evaluates whether the delimiter and the sequence are equivalent to the extent of the delimiter
    static bool is_equal_(ss_char_w_t const delim, const_iterator &it)
    {
        return delim == *it;
    }
    /// Returns the length of the delimiter
    static ss_size_t get_length_(ss_char_a_t const /* delim */)
    {
        return 1;
    }
    /// Returns the length of the delimiter
    static ss_size_t get_length_(ss_char_w_t const /* delim */)
    {
        return 1;
    }

    static const_iterator advance_(const_iterator it, delimiter_type const& delim)
    {
        return it + get_length_(delim);
    }

public:
    /// Evaluates whether the delimiter and the sequence are not equivalent to the extent of the delimiter
    static bool not_equal(delimiter_type const& delim, const_iterator &it)
    {
        return !is_equal_(delim, it);
    }

    /// Returns the length of the delimiter
    static ss_size_t length(delimiter_type const& delim)
    {
        return get_length_(delim);
    }

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
    static bool test_start_token_advance(const_iterator &it, const_iterator end, delimiter_type const& delim)
    {
        return is_equal_(delim, it) ? (it = advance_(it, delim), true) : false;
    }

    static bool test_end_token_advance(const_iterator &it, const_iterator end, delimiter_type const& delim)
    {
        return is_equal_(delim, it) ? (it = advance_(it, delim), true) : false;
    }

    static const_iterator nonskip_move_to_start(const_iterator it, const_iterator end, delimiter_type const& delim)
    {
        return it;
    }

    static bool test_end_token(const_iterator it, const_iterator end, delimiter_type const& delim)
    {
        return is_equal_(delim, it);
    }

    static const_iterator find_next_start(const_iterator it, const_iterator end, delimiter_type const& delim)
    {
        return advance_(it, delim);
    }
#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */
/// @}
};

/** A class template that provides string tokenising behaviour
 *
 * \ingroup group__library__String
 *
 * This class takes a string, and a delimiter, and fashions a sequence from
 * the given string, with each element determined with respect to the
 * delimiter
 *
 * \param S The string type
 * \param D The delimiter type (can be a string type or a character type)
 * \param B The ignore-blanks type. Defaults to skip_blank_tokens&lt;true&gt;
 * \param V The value type (the string type that will be used for the values). Defaults to \c S
 * \param T The string type traits type. Defaults to string_tokeniser_type_traits&lt;S, V&gt;
 * \param P The tokeniser comparator type. Defaults to string_tokeniser_comparator&lt;D, S, T&gt;
 *
 * This class template provides tokenising services of a string (of type \c S)
 * with a delimiter (of type \c D). The four other template parameters, which are
 * defaulted, are used for tailoring the tokenising behaviour for special uses.
 *
 * The two typical supported tokenising scenarios are:
 *
 *  - tokenising a string with a character (e.g. '\\n')
 *  - tokenising a string with a string (e.g. "\\r\\n")
 *
 * More exotic scenarios are supported by customising the comparator and type-traits
 * parameters. (See stlsoft::charset_tokeniser.)
 *
 * <b>1. Tokenising a string with a character.</b>
 *
 * This uses a specialisation whereby the first template parameter is a string type,
 * and the second parameter is a corresponding character type.
 *
 * The following code shows a specialisation using std::string and char, and
 * will output: <b>abc,def,ghi,jkl,</b>
 *
\code

stlsoft::string_tokeniser<std::string, char>  tokens(":abc::def:ghi:jkl::::::::::", ':');

std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<std::string>(std::cout, ","));
\endcode
 *
 * The following code shows a specialisation using
 * stlsoft::basic_simple_string&lt;wchar_t&gt; and wchar_t, and
 * will output: <b>abc-def-ghi-jkl-</b>
 *
\code

typedef stlsoft::basic_simple_string<wchar_t>   string_t;
string_t                                        s(L"|abc||def|ghi|jkl||||||||||");
stlsoft::string_tokeniser<string_t, wchar_t>    tokens(s, L'|');

std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<string_t, wchar_t>(std::wcout, L"-"));
\endcode
 *
 * Optionally, you can stipulate that the blanks be retained by specifying the third
 * template parameter as skip_blank_tokens<false>, as in the following, which will
 * output: <b>,abc,,def,ghi,jkl,,,,,,,,,,</b>
 *
\code

stlsoft::string_tokeniser<  std::string
                        ,   char
                        ,   stlsoft::skip_blank_tokens<false>
                        >                tokens(":abc::def:ghi:jkl::::::::::", ':');

std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<std::string>(std::cout, ","));
\endcode
 *
 * \note The tokeniser uses \ref group__concept__Shim__string_access "String Access Shims" to elicit the
 * string from the given type, so any type that for which shims are defined can be passed to the
 * constructor, as in the following, which will output: <b>abc;def;ghi;jkl;</b>
 *
\code

#include <stlsoft/string/string_tokeniser.hpp>
#include <winstl/shims/access/string.hpp>

#include <iostream>
#include <iterator>

int main()
{
  HWND  hwndButton = ::CreateWindowEx(0, "BUTTON", "+abc++def+ghi+jkl++++++++++", 0, 0, 0, 0, 0, NULL, (HMENU)0, NULL, NULL);

  stlsoft::string_tokeniser<  std::string
                          ,   char
                          ,   stlsoft::skip_blank_tokens<true>
                          >                tokens(hwndButton, '+');
  std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<std::string>(std::cout, ";"));
  return 0;
}
\endcode
 *
 * <b>2. Tokenising a string with a string.</b>
 *
 * This uses a specialisation whereby the first template parameter is a string type,
 * and the second parameter is a corresponding string type.
 *
 * The following code shows a specialisation using std::string and std::string, and
 * will output: <b>abc,def,ghi,jkl,</b>
 *
\code

stlsoft::string_tokeniser<std::string, std::string>  tokens("\r\nabc\r\n\r\ndef\r\nghi\r\njkl\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n", "\r\n");

std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<std::string>(std::cout, ","));
\endcode
 */
template< typename S
        , typename D
        , typename B = skip_blank_tokens<true>
        , typename V = S
        , typename T = string_tokeniser_type_traits<S, V>
        , typename P = string_tokeniser_comparator<D, S, T>
        >
class string_tokeniser
    : public stl_collection_tag
{
/// \name Member Types
/// @{
public:
    /// The current parameterisation of the type
    typedef string_tokeniser<S, D, B, V, T, P>              class_type;
    /// This tokeniser parameterisation
    typedef string_tokeniser<S, D, B, V, T, P>              tokeniser_type;
    /// The sequence string type
    typedef S                                               string_type;
    /// The delimiter type
    typedef D                                               delimiter_type;
    /// The blanks policy type
    typedef B                                               blanks_policy_type;
    /// The value type
    typedef V                                               value_type;
    /// The traits type
    typedef T                                               traits_type;
    /// The tokeniser comparator type
    typedef P                                               comparator_type;
    /// The character type
    typedef typename traits_type::value_type                char_type;
    /// The size type
    ///
    /// \note This no longer relies on a size_type member type of the traits type (T). It is defined
    /// as size_t
    typedef ss_size_t                                       size_type;
    /// The difference type
    ///
    /// \note This no longer relies on a difference_type member type of the traits type (T). It is defined
    /// as ptrdiff_t
    typedef ss_ptrdiff_t                                    difference_type;
    /// The non-mutating (const) reference type
    typedef const value_type                                const_reference;
    /// The non-mutating (const) iterator type
    class                                                   const_iterator;
/// @}

/// \name Construction
/// @{
public:
    /// Tokenise the given C-string with the given delimiter
    ///
    /// \param psz Pointer to C-string whose contents will be tokenised
    /// \param delim The delimiter to perform the tokenisation
    ///
    /// \note The tokeniser class takes a copy of \c psz. It does not alter the contents of \c psz
    string_tokeniser(char_type const* psz, delimiter_type const& delim)
        : m_str(psz)
        , m_delimiter(delim)
    {
        ZYSOFT_MESSAGE_ASSERT("Delimiter of zero-length", 0 != comparator_type::length(m_delimiter));

        ZYSOFT_ASSERT(is_valid());
    }

    /// Tokenise the given string with the given delimiter
    ///
    /// \param str The string whose contents will be tokenised
    /// \param delim The delimiter to perform the tokenisation
    ///
    /// \note The tokeniser class takes a copy of \c str. It does not alter the contents of \c str
    string_tokeniser(string_type const& str, delimiter_type const& delim)
        : m_str(str)
        , m_delimiter(delim)
    {
        ZYSOFT_MESSAGE_ASSERT("Delimiter of zero-length", 0 != comparator_type::length(m_delimiter));
        ZYSOFT_ASSERT(is_valid());
    }

// Define the template overload if member template ctors are supported
    /// Tokenise the given string with the given delimiter
    ///
    /// \param str The string whose contents will be tokenised
    /// \param delim The delimiter to perform the tokenisation
    ///
    /// \note The tokeniser class takes a copy of \c str. It does not alter the contents of \c str
    template <ss_typename_param_k S1>
    string_tokeniser(S1 const& str, delimiter_type const& delim)
        : m_str(c_str_data(str), c_str_len(str))
        , m_delimiter(delim)
    {
        ZYSOFT_MESSAGE_ASSERT("Delimiter of zero-length", 0 != comparator_type::length(m_delimiter));

        ZYSOFT_ASSERT(is_valid());
    }

    /// Tokenise the specified length of the given string with the given delimiter
    ///
    /// \param psz Pointer to C-string whose contents will be tokenised
    /// \param cch The number of characters in \c psz to use
    /// \param delim The delimiter to perform the tokenisation
    ///
    /// \note The tokeniser class takes a copy of \c psz. It does not alter the contents of \c psz
    string_tokeniser(char_type const* psz, size_type cch, delimiter_type const& delim)
        : m_str(psz, cch)
        , m_delimiter(delim)
    {
        ZYSOFT_MESSAGE_ASSERT("Delimiter of zero-length", 0 != comparator_type::length(m_delimiter));

        ZYSOFT_ASSERT(is_valid());
    }

    /// Tokenise the given range with the given delimiter
    ///
    /// \param from The start of the asymmetric range to tokenise
    /// \param to The start of the asymmetric range to tokenise
    /// \param delim The delimiter to use
    string_tokeniser(char_type const* from, char_type const* to, delimiter_type const& delim)
        : m_str(from, to)
        , m_delimiter(delim)
    {
        ZYSOFT_MESSAGE_ASSERT("Delimiter of zero-length", 0 != comparator_type::length(m_delimiter));

        ZYSOFT_ASSERT(is_valid());
    }

    /// Tokenise the given range with the given delimiter
    ///
    /// \param from The start of the asymmetric range to tokenise
    /// \param to The start of the asymmetric range to tokenise
    /// \param delim The delimiter to use
    template <ss_typename_param_k I>
    string_tokeniser(I from, I to, delimiter_type const& delim)
        : m_str(from, to)
        , m_delimiter(delim)
    {
        ZYSOFT_MESSAGE_ASSERT("Delimiter of zero-length", 0 != comparator_type::length(m_delimiter));

        ZYSOFT_ASSERT(is_valid());
    }
/// @}

/// \name Iteration
/// @{
public:
    /// Iterator for string_tokeniser, supporting the Forward Iterator concept
    class const_iterator
        : public iterator_base< STLSOFT_NS_QUAL_STD(forward_iterator_tag)
                            ,   value_type
                            ,   ss_ptrdiff_t
                            ,   void
                            ,   value_type
                            >
    {
    /// \name Member Types
    /// @{
    public:
        /// The type
        typedef const_iterator                                      class_type;
        /// The delimiter type
        typedef typename tokeniser_type::delimiter_type             delimiter_type;
        /// The value type
        typedef typename tokeniser_type::value_type                 value_type;
        /// The traits type
        typedef typename tokeniser_type::traits_type                traits_type;
        typedef value_type                                          effective_const_reference;
    private:
        typedef typename traits_type::const_iterator_type           underlying_iterator_type;
# if defined(STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION)
        typedef delimiter_type const*                               delimiter_ref_type;
# else /* ? STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
        typedef delimiter_type                                      delimiter_ref_type;
# endif /* STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
    /// @}

    /// \name Construction
    /// @{
    private:
        friend class    string_tokeniser<S, D, B, V, T, P>;

        /// Conversion constructor
        const_iterator(underlying_iterator_type first, underlying_iterator_type last, delimiter_type const& delimiter)
            : m_find0(first)
            , m_find1(first)
            , m_next(first)
            , m_end(last)
# if defined(STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION)
            , m_delimiter(&delimiter)
# else /* ? STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
            , m_delimiter(delimiter)
# endif /* STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
            , m_cchDelimiter(comparator_type::length(delimiter))
        {
            if(m_end != m_find0)
            {
                increment_();
            }
        }
    public:
        /// Default constructor
        const_iterator()
            : m_find0(NULL)
            , m_find1(NULL)
            , m_next(NULL)
            , m_end(NULL)
            , m_delimiter(delimiter_ref_type())
            , m_cchDelimiter(0)
        {}

        /// Copy constructor
        ///
        /// \param rhs The iterator whose current search position will be copied
        const_iterator(class_type const& rhs)
            : m_find0(rhs.m_find0)
            , m_find1(rhs.m_find1)
            , m_next(rhs.m_next)
            , m_end(rhs.m_end)
            , m_delimiter(rhs.m_delimiter)
            , m_cchDelimiter(comparator_type::length(get_delim_ref_(rhs.m_delimiter)))
        {}

        /// Copy-assignment operator
        ///
        /// \param rhs The iterator whose current search position will be copied
        class_type const& operator =(class_type const& rhs)
        {
            m_find0         =   rhs.m_find0;
            m_find1         =   rhs.m_find1;
            m_next          =   rhs.m_next;
            m_end           =   rhs.m_end;
            m_delimiter     =   rhs.m_delimiter;
            m_cchDelimiter  =   rhs.m_cchDelimiter;

            return *this;
        }
    /// @}

    /// \name Forward Iterator Methods
    /// @{
    public:
        /// Dereference operator
        //
        // This has to be V, rather than value_type, because Visual C++ thinks that S is the value_type!!
        const V operator *() const
        {
            return traits_type::create(m_find0, m_find1);
        }

        /// Pre-increment operator
        class_type& operator ++()
        {
            increment_();

            return *this;
        }

        /// Post-increment operator
        const class_type operator ++(int)
        {
            class_type  ret(*this);

            operator ++();

            return ret;
        }

        /// Evaluates whether \c this and \c rhs are equivalent
        bool equal(class_type const& rhs) const
        {
            ZYSOFT_MESSAGE_ASSERT("Comparing iterators from different tokenisers", m_end == rhs.m_end);

            return m_find0 == rhs.m_find0;
        }

        /// Evaluates whether \c this and \c rhs are equivalent
        bool operator == (class_type const& rhs) const
        {
            return equal(rhs);
        }

        /// Evaluates whether \c this and \c rhs are not equivalent
        bool operator != (class_type const& rhs) const
        {
            return !equal(rhs);
        }
    /// @}

    /// \name Implementation
    /// @{
    private:
        static delimiter_type const& get_delim_ref_(delimiter_ref_type const& delim)
        {
# if defined(STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION)
            return *delim;
# else /* ? STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
            return delim;
# endif /* STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
        }

        void increment_()
        {
            ZYSOFT_MESSAGE_ASSERT("Attempting to increment an invalid iterator", m_find0 != m_end);

            // This is a two-phase algorithm:
            //
            // 1. If skipping blanks, then do that. Otherwise, locate the the start-of-item
            //     iterator (m_find0) to the previously identified start of the next item (m_next)
            // 2. Starting from m_find0, determine the end-of-item (m_find1)

            skip_blanks_if_(skip_discriminator_type_<blanks_policy_type::value != 0>());

            determine_end_();
        }

        void skip_blanks_if_(skip_discriminator_type_<1>)
        {
            // 1. Skip blanks until at start of next item
            for(m_find0 = m_next; m_find0 != m_end; )
            {
                if(comparator_type::not_equal(get_delim_ref_(m_delimiter), m_find0))
                {
                    break;
                }
                else
                {
                    m_find0 +=  static_cast<ss_ptrdiff_t>(m_cchDelimiter);
                }
            }
        }

        void skip_blanks_if_(skip_discriminator_type_<0>)
        {
            m_find0 = m_next;
        }

        void determine_end_()
        {
            // 2. Determine the end-of-item (m_find1), starting from m_find0
            for(m_find1 = m_find0; ; )
            {
                if(m_find1 == m_end)
                {
                    // End of sequence. Item will be [m_find0, m_end (== m_find1))
                    m_next = m_find1;
                    break;
                }
                else if(comparator_type::not_equal(get_delim_ref_(m_delimiter), m_find1))
                {
                    // current item does not hold a delimiter, so advance one position
                    ++m_find1;
                }
                else
                {
                    // Determine the start of the next potential element, ready
                    // for the next call of increment_()
                    m_next = m_find1 + static_cast<ss_ptrdiff_t>(m_cchDelimiter);

                    break;
                }
            }
        }
    /// @}

    /// \name Members
    /// @{
    private:
        underlying_iterator_type    m_find0;        // the start of the current item
        underlying_iterator_type    m_find1;        // the end of the current item
        underlying_iterator_type    m_next;         // the start of the next valid (non-null) item
        underlying_iterator_type    m_end;          // end point of controlled sequence
        delimiter_ref_type          m_delimiter;    // The delimiter
        std::size_t                 m_cchDelimiter;
    /// @}
    };

    /// Begins the iteration
    ///
    /// \return An iterator representing the start of the sequence
    const_iterator begin() const
    {
        ZYSOFT_ASSERT(is_valid());

        return const_iterator(traits_type::begin(m_str), traits_type::end(m_str), m_delimiter);
    }
    /// Ends the iteration
    ///
    /// \return An iterator representing the end of the sequence
    const_iterator end() const
    {
        ZYSOFT_ASSERT(is_valid());

        return const_iterator(traits_type::end(m_str), traits_type::end(m_str), m_delimiter);
    }
/// @}

/// \name Attributes
/// @{
public:
    /// Indicates whether the search sequence is empty
    bool empty() const
    {
        ZYSOFT_ASSERT(is_valid());

        return begin() == end();
    }
/// @}

/// \name Invariant
/// @{
private:
    bool is_valid() const
    {
        return true;
    }
/// @}

/// \name Members
/// @{
private:
    string_type const       m_str;
    delimiter_type const    m_delimiter;
/// @}

/// \name Not to be implemented
/// @{
private:
    class_type const& operator =(class_type const&);
/// @}
};


template <bool B>
struct skip_blank_tokens
{
    enum { value = B };
};

template< typename S
        , typename D
        , typename B = skip_blank_tokens<true>
        , typename V = S
        >
class string_tokeniser
    : public stl_collection_tag
{
public:
    /// The current parameterisation of the type
    typedef string_tokeniser<S, D, B, V>                    class_type;
    /// The sequence string type
    typedef S                                               string_type;
    /// The delimiter type
    typedef D                                               delimiter_type;
    /// The blanks policy type
    typedef B                                               blanks_policy_type;
    /// The value type
    typedef V                                               value_type;
    /// The size type
    ///
    /// \note This no longer relies on a size_type member type of the traits type (T). It is defined
    /// as size_t
    typedef std::size_t                                     size_type;
    /// The difference type
    ///
    /// \note This no longer relies on a difference_type member type of the traits type (T). It is defined
    /// as ptrdiff_t
    typedef std::ptrdiff_t                                 difference_type;
    /// The non-mutating (const) reference type
    typedef const value_type                                const_reference;
    /// The non-mutating (const) iterator type
    class                                                   const_iterator;
public:

    class const_iterator
        : public iterator_base< STLSOFT_NS_QUAL_STD(forward_iterator_tag)
        , value_type
        , ss_ptrdiff_t
        , void
        , value_type
        >
    {
        /// \name Member Types
        /// @{
    public:
        /// The type
        typedef const_iterator                                      class_type;
        /// The delimiter type
        typedef typename tokeniser_type::delimiter_type             delimiter_type;
        /// The value type
        typedef typename tokeniser_type::value_type                 value_type;
        /// The traits type
        typedef typename tokeniser_type::traits_type                traits_type;
        typedef value_type                                          effective_const_reference;
    private:
        typedef typename traits_type::const_iterator_type           underlying_iterator_type;
# if defined(STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION)
        typedef delimiter_type const* delimiter_ref_type;
# else /* ? STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
        typedef delimiter_type                                      delimiter_ref_type;
# endif /* STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
        /// @}

        /// \name Construction
        /// @{
    private:
        friend class    string_tokeniser<S, D, B, V, T, P>;

        /// Conversion constructor
        const_iterator(underlying_iterator_type first, underlying_iterator_type last, delimiter_type const& delimiter)
            : m_find0(first)
            , m_find1(first)
            , m_next(first)
            , m_end(last)
# if defined(STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION)
            , m_delimiter(&delimiter)
# else /* ? STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
            , m_delimiter(delimiter)
# endif /* STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
            , m_cchDelimiter(comparator_type::length(delimiter))
        {
            if (m_end != m_find0)
            {
                increment_();
            }
        }
    public:
        /// Default constructor
        const_iterator()
            : m_find0(NULL)
            , m_find1(NULL)
            , m_next(NULL)
            , m_end(NULL)
            , m_delimiter(delimiter_ref_type())
            , m_cchDelimiter(0)
        {}

        /// Copy constructor
        ///
        /// \param rhs The iterator whose current search position will be copied
        const_iterator(class_type const& rhs)
            : m_find0(rhs.m_find0)
            , m_find1(rhs.m_find1)
            , m_next(rhs.m_next)
            , m_end(rhs.m_end)
            , m_delimiter(rhs.m_delimiter)
            , m_cchDelimiter(comparator_type::length(get_delim_ref_(rhs.m_delimiter)))
        {}

        /// Copy-assignment operator
        ///
        /// \param rhs The iterator whose current search position will be copied
        class_type const& operator =(class_type const& rhs)
        {
            m_find0 = rhs.m_find0;
            m_find1 = rhs.m_find1;
            m_next = rhs.m_next;
            m_end = rhs.m_end;
            m_delimiter = rhs.m_delimiter;
            m_cchDelimiter = rhs.m_cchDelimiter;

            return *this;
        }
        /// @}

        /// \name Forward Iterator Methods
        /// @{
    public:
        /// Dereference operator
        //
        // This has to be V, rather than value_type, because Visual C++ thinks that S is the value_type!!
        const V operator *() const
        {
            return traits_type::create(m_find0, m_find1);
        }

        /// Pre-increment operator
        class_type& operator ++()
        {
            increment_();

            return *this;
        }

        /// Post-increment operator
        const class_type operator ++(int)
        {
            class_type  ret(*this);

            operator ++();

            return ret;
        }

        /// Evaluates whether \c this and \c rhs are equivalent
        bool equal(class_type const& rhs) const
        {
            ZYSOFT_MESSAGE_ASSERT("Comparing iterators from different tokenisers", m_end == rhs.m_end);

            return m_find0 == rhs.m_find0;
        }

        /// Evaluates whether \c this and \c rhs are equivalent
        bool operator == (class_type const& rhs) const
        {
            return equal(rhs);
        }

        /// Evaluates whether \c this and \c rhs are not equivalent
        bool operator != (class_type const& rhs) const
        {
            return !equal(rhs);
        }
        /// @}

        /// \name Implementation
        /// @{
    private:
        static delimiter_type const& get_delim_ref_(delimiter_ref_type const& delim)
        {
# if defined(STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION)
            return *delim;
# else /* ? STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
            return delim;
# endif /* STLSOFT_STRING_TOKENISER_CF_REQUIRE_DELIMITER_INDIRECTION */
        }

        void increment_()
        {
            ZYSOFT_MESSAGE_ASSERT("Attempting to increment an invalid iterator", m_find0 != m_end);

            // This is a two-phase algorithm:
            //
            // 1. If skipping blanks, then do that. Otherwise, locate the the start-of-item
            //     iterator (m_find0) to the previously identified start of the next item (m_next)
            // 2. Starting from m_find0, determine the end-of-item (m_find1)

            skip_blanks_if_(skip_discriminator_type_<blanks_policy_type::value != 0>());

            determine_end_();
        }

        void skip_blanks_if_(skip_discriminator_type_<1>)
        {
            // 1. Skip blanks until at start of next item
            for (m_find0 = m_next; m_find0 != m_end; )
            {
                if (comparator_type::not_equal(get_delim_ref_(m_delimiter), m_find0))
                {
                    break;
                }
                else
                {
                    m_find0 += static_cast<ss_ptrdiff_t>(m_cchDelimiter);
                }
            }
        }

        void skip_blanks_if_(skip_discriminator_type_<0>)
        {
            m_find0 = m_next;
        }

        void determine_end_()
        {
            // 2. Determine the end-of-item (m_find1), starting from m_find0
            for (m_find1 = m_find0; ; )
            {
                if (m_find1 == m_end)
                {
                    // End of sequence. Item will be [m_find0, m_end (== m_find1))
                    m_next = m_find1;
                    break;
                }
                else if (comparator_type::not_equal(get_delim_ref_(m_delimiter), m_find1))
                {
                    // current item does not hold a delimiter, so advance one position
                    ++m_find1;
                }
                else
                {
                    // Determine the start of the next potential element, ready
                    // for the next call of increment_()
                    m_next = m_find1 + static_cast<ss_ptrdiff_t>(m_cchDelimiter);

                    break;
                }
            }
        }
        /// @}

        /// \name Members
        /// @{
    private:
        underlying_iterator_type    m_find0;        // the start of the current item
        underlying_iterator_type    m_find1;        // the end of the current item
        underlying_iterator_type    m_next;         // the start of the next valid (non-null) item
        underlying_iterator_type    m_end;          // end point of controlled sequence
        delimiter_ref_type          m_delimiter;    // The delimiter
        std::size_t                 m_cchDelimiter;
        /// @}
    }

};
#endif

} // namespace zysoft


