#pragma once

#include <vector>

#include "NetworkType.h"
#include "CallbackMessage.h"

namespace network {

void callbackMessageDecoder(Hdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
CallbackMessagePtr callbackMessageEncoder(int32_t msg_id, const std::vector<uint8_t>& data);

} // network
