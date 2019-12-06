#pragma once

#include <cstdint>
#include <array>

#include "zysoft/string/detail/ngx_md5.h"

namespace zysoft {

using MD5Context = detail::ngx_md5_t;

inline void MD5_Init(MD5Context* ctx)
{
    detail::ngx_md5_init(ctx);
}

inline void MD5_Update(MD5Context* ctx, const void *data, std::size_t size)
{
    detail::ngx_md5_update(ctx, data, size);
}

inline void MD5_Final(std::array<std::uint8_t, 16>* out, MD5Context* ctx)
{
    detail::ngx_md5_final(out->data(), ctx);
}


} // namespace zysoft
