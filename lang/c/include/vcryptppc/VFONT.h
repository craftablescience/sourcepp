#pragma once

#include <sourceppc/Buffer.h>

SOURCEPP_STATIC(vcryptpp, vfont, sourcepp_buffer_t, encrypt, const unsigned char* buffer, size_t bufferLen, uint8_t saltLen); // REQUIRES MANUAL FREE: sourcepp_buffer_free

SOURCEPP_STATIC(vcryptpp, vfont, sourcepp_buffer_t, decrypt, const unsigned char* buffer, size_t bufferLen); // REQUIRES MANUAL FREE: sourcepp_buffer_free
