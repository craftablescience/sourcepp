#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "ImageFormats.h"

typedef void* vtfpp_psframes_handle_t;

SOURCEPP_API vtfpp_psframes_handle_t vtfpp_psframes_open_from_mem(const unsigned char* buffer, size_t bufferLen); // REQUIRES MANUAL FREE: vtfpp_psframes_close
SOURCEPP_API vtfpp_psframes_handle_t vtfpp_psframes_open_from_file(const char* psframesPath); // REQUIRES MANUAL FREE: vtfpp_psframes_close
SOURCEPP_API void vtfpp_psframes_close(vtfpp_psframes_handle_t* handle);
SOURCEPP_API uint32_t vtfpp_psframes_get_frame_count(vtfpp_psframes_handle_t handle);
SOURCEPP_API uint32_t vtfpp_psframes_get_fps(vtfpp_psframes_handle_t handle);
SOURCEPP_API uint16_t vtfpp_psframes_get_width(vtfpp_psframes_handle_t handle);
SOURCEPP_API uint16_t vtfpp_psframes_get_height(vtfpp_psframes_handle_t handle);
SOURCEPP_API sourcepp_buffer_t vtfpp_psframes_get_image_data_as(vtfpp_psframes_handle_t handle, vtfpp_image_format_e newFormat, uint32_t frame); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_psframes_get_image_data_as_bgr888(vtfpp_psframes_handle_t handle, uint32_t frame); // REQUIRES MANUAL FREE: sourcepp_buffer_free

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/PSFrames.h>

#endif
