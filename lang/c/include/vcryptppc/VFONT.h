#pragma once

#include <sourceppc/Buffer.h>

SOURCEPP_EXTERNVAR const char* VCRYPTPP_VFONT_SIGNATURE;

SOURCEPP_EXTERNVAR const uint8_t VCRYPTPP_VFONT_MAGIC;

SOURCEPP_API sourcepp_buffer_t vcryptpp_vfont_encrypt(const unsigned char* buffer, size_t bufferLen, uint8_t saltLen); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vcryptpp_vfont_decrypt(const unsigned char* buffer, size_t bufferLen); // REQUIRES MANUAL FREE: sourcepp_buffer_free

// C++ conversion routines
#ifdef __cplusplus

#include <vcryptpp/VFONT.h>

#endif
