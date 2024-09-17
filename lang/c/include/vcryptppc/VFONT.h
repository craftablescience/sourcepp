#pragma once

#include <sourceppc/Buffer.h>

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vcryptpp_vfont_decrypt(const unsigned char* buffer, size_t bufferLen);
