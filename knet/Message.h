#pragma once

#include <vector>

#include "knet/KNetTypes.h"

namespace knet {

struct MessageDecoder
{
    static void encode(MessagePtr msg, std::vector<uint8_t>* out);
    static MessagePtr decode(const uint8_t* p, size_t len, const uint8_t* key, size_t key_len);
};

} // knet
