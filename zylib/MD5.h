#pragma once

#include <cstdint>
#include <array>

#include "zylib/hashing/ngx_md5.h"

namespace zylib {

using MD5Context = detail::ngx_md5_t;

void MD5_Init(MD5Context* ctx);
void MD5_Update(MD5Context* ctx, const void *data, std::size_t size);
void MD5_Final(std::array<std::uint8_t, 16>* out, MD5Context* ctx);


} // namespace zylib
