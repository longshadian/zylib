#pragma once

namespace zysoft
{


template< typename I
        , typename O
        >
void hex_dump(I b, I e, O o)
{
    static const std::uint8_t hex[] = "0123456789abcdef";
    while (b != e) {
        *o++ = hex[*b >> 4];
        *o++ = hex[*b++ & 0xf];
    }
}

}
