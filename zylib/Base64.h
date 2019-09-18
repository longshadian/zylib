#pragma once

#include <string>

namespace zylib {
namespace base64 {

std::string Encode(const std::string& s);
std::string Encode(const unsigned char* p, unsigned int len);
std::string Decode(const std::string& s);

} // namespace base64 
} // namepsace zylib
