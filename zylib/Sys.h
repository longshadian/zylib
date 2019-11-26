#pragma once

#include <cstdio>

namespace zylib {

void Fopen(std::FILE** f, const char* fname, const char* mod);
std::FILE* Fopen(const char* fname, const char* mod);

} // namespace zylib
