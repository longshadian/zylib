#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace zysoft {
namespace base64 {

inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

inline std::size_t encoded_length(std::size_t len)
{
    return ((len + 2) / 3) * 4;
}

inline std::size_t decoded_length(std::size_t len)
{
    return ((len + 3) / 4) * 3;
}

void Base64Encode(const void* src, std::size_t src_len, void* dst, std::size_t* dst_len)
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

std::string Encode(const unsigned char* p, unsigned int len);
std::string Decode(const std::string& s);

} // namespace base64 
} // namepsace zysoft
