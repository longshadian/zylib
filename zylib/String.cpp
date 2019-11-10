#include "zylib/String.h"

#include <vector>

#include "zylib/hashing/ngx_string.h"

namespace zylib {

std::uint8_t* HexDump(std::uint8_t* dst, std::uint8_t* src, std::size_t len)
{
    static uint8_t hex[] = "0123456789abcdef";
    while (len--) {
        *dst++ = hex[*src >> 4];
        *dst++ = hex[*src++ & 0xf];
    }
    return dst;
}

#if 0
std::size_t Base64Encode(const std::uint8_t* src, std::size_t src_len, void* dst, std::size_t* dst_len)
{
    static std::uint8_t   basis64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const std::uint8_t* s = src;
    std::size_t len = src_len;
    std::uint8_t* d = reinterpret_cast<std::uint8_t*>(dst);

    while (len > 2) {
        *d++ = basis64[(s[0] >> 2) & 0x3f];
        *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
        *d++ = basis64[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = basis64[s[2] & 0x3f];

        s += 3;
        len -= 3;
    }

    if (len) {
        *d++ = basis64[(s[0] >> 2) & 0x3f];

        if (len == 1) {
            *d++ = basis64[(s[0] & 3) << 4];
            *d++ = '=';

        }
        else {
            *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = basis64[(s[1] & 0x0f) << 2];
        }

        *d++ = '=';
    }

    std::size_t flen = d - reinterpret_cast<std::uint8_t*>(dst);
    if (dst_len)
        *dst_len = flen;
    return flen;
}
#endif

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

void Base64Encode(const void* src, std::size_t src_len, std::string* dst)
{
    std::size_t buffer_len = Base64EncodedLength(src_len);
    dst->resize(buffer_len);
    Base64Encode(src, src_len, dst->data(), &buffer_len);
    dst->resize(buffer_len);
}

std::int32_t Base64Decode(const void* src, std::size_t src_len, void* dst, std::size_t* dst_len)
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


} // namespace zylib
