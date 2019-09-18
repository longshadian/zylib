#pragma once

namespace zylib {

void CRC32_InitChecksum( unsigned long &crcvalue );
void CRC32_UpdateChecksum( unsigned long &crcvalue, const void *data, int length );
void CRC32_FinishChecksum( unsigned long &crcvalue );
unsigned long CRC32_BlockChecksum( const void *data, int length );

} // namespace zylib
