#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include "zysoft/string/detail/ngx_base64.h"

namespace zysoft 
{

namespace base64 
{

inline bool is_base64(unsigned char c) {
	return (std::isalnum(c) || (c == '+') || (c == '/'));
}

inline std::size_t encoded_length(std::size_t len)
{
    return ((len + 2) / 3) * 4;
}

inline std::size_t decoded_length(std::size_t len)
{
    return ((len + 3) / 4) * 3;
}

inline void Base64Encode(const void* src, std::size_t src_len, void* dst, std::size_t* dst_len)
{
    detail::ngx_str_t in_src{};
    in_src.data = (detail::u_char*)src;
    in_src.len = src_len;

    detail::ngx_str_t out_dst{};
    out_dst.data = (detail::u_char*)dst;
    out_dst.len = 0;
    detail::ngx_encode_base64(&out_dst, &in_src);
    if (dst_len)
        *dst_len = out_dst.len;
}

inline void Base64Encode(const void* src, std::size_t src_len, std::string* dst)
{
    std::size_t buffer_len = encoded_length(src_len);
    dst->resize(buffer_len);
    Base64Encode(src, src_len, dst->data(), &buffer_len);
    dst->resize(buffer_len);
}

inline std::int32_t Base64Decode(const void* src, std::size_t src_len, void* dst, std::size_t* dst_len)
{
    detail::ngx_str_t in_src{};
    in_src.data = (detail::u_char*)src;
    in_src.len = src_len;

    detail::ngx_str_t out_dst{};
    out_dst.data = (detail::u_char*)dst;
    out_dst.len = 0;
    std::int32_t code = detail::ngx_decode_base64(&out_dst, &in_src);
    if (dst_len)
        *dst_len = out_dst.len;
    return code;
}
} // namespace base64 
} // namepsace zysoft
