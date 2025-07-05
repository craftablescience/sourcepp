#pragma once

#include <sourceppc/Buffer.h>

SOURCEPP_STATIC(vcryptpp, vfont, sourcepp_buffer_t, decrypt, const unsigned char* buffer, size_t bufferLen); // REQUIRES MANUAL FREE: sourcepp_buffer_free
