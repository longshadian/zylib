/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
#pragma once

#include <cstdint>

namespace zylib {
namespace detail {

typedef struct {
    std::uint64_t   bytes;
    std::uint32_t   a, b, c, d;
    std::uint8_t    buffer[64];
} ngx_md5_t;


void ngx_md5_init(ngx_md5_t *ctx);
void ngx_md5_update(ngx_md5_t *ctx, const void *data, size_t size);
void ngx_md5_final(std::uint8_t result[16], ngx_md5_t *ctx);

} // namespace detail
} // namespace zylib

