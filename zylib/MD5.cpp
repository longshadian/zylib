#include "zylib/MD5.h"

namespace zylib
{

void MD5_Init(MD5Context* ctx)
{
    detail::ngx_md5_init(ctx);
}

void MD5_Update(MD5Context* ctx, const void *data, std::size_t size)
{
    detail::ngx_md5_update(ctx, data, size);
}

void MD5_Final(std::array<std::uint8_t, 16>* out, MD5Context* ctx)
{
    detail::ngx_md5_final(out->data(), ctx);
}

} // namespace zylib
