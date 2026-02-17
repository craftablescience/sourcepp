#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

SOURCEPP_API sourcepp_buffer_t vtfpp_image_quantize_convert_p8_image_data_to_bgra8888(const unsigned char* paletteData, size_t paletteDataLen, const unsigned char* imageData, size_t imageDataLen);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/ImageQuantize.h>

#endif
