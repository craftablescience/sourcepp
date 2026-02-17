#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "ImageConversion.h"

typedef void* vtfpp_ppl_handle_t;

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create(uint32_t modelChecksum); // REQUIRES MANUAL FREE: vtfpp_ppl_close
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create_with_options(uint32_t modelChecksum, vtfpp_image_format_e format, uint32_t version); // REQUIRES MANUAL FREE: vtfpp_ppl_close
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_open_from_mem(const unsigned char* buffer, size_t bufferLen); // REQUIRES MANUAL FREE: vtfpp_ppl_close
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_open_from_file(const char* pplPath); // REQUIRES MANUAL FREE: vtfpp_ppl_close
SOURCEPP_API void vtfpp_ppl_close(vtfpp_ppl_handle_t* handle);
SOURCEPP_API int vtfpp_ppl_is_valid(vtfpp_ppl_handle_t handle);
SOURCEPP_API uint32_t vtfpp_ppl_get_version(vtfpp_ppl_handle_t handle);
SOURCEPP_API void vtfpp_ppl_set_version(vtfpp_ppl_handle_t handle, uint32_t version);
SOURCEPP_API uint32_t vtfpp_ppl_get_model_checksum(vtfpp_ppl_handle_t handle);
SOURCEPP_API void vtfpp_ppl_set_model_checksum(vtfpp_ppl_handle_t handle, uint32_t modelChecksum);
SOURCEPP_API vtfpp_image_format_e vtfpp_ppl_get_format(vtfpp_ppl_handle_t handle);
SOURCEPP_API void vtfpp_ppl_set_format(vtfpp_ppl_handle_t handle, vtfpp_image_format_e format, float quality);
SOURCEPP_API int vtfpp_ppl_has_image_for_lod(vtfpp_ppl_handle_t handle, uint32_t lod);
SOURCEPP_API sourcepp_buffer_uint32_t vtfpp_ppl_get_image_lods(vtfpp_ppl_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_raw(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, vtfpp_image_format_e format, uint32_t lod); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as_rgb888(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API int vtfpp_ppl_set_image_from_file(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t lod, float quality);
SOURCEPP_API int vtfpp_ppl_set_image_from_file_with_options(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, vtfpp_image_conversion_resize_filter_e filter, float quality);
SOURCEPP_API int vtfpp_ppl_set_image_from_mem(vtfpp_ppl_handle_t handle, const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint32_t width, uint32_t height, uint32_t lod, float quality);
SOURCEPP_API int vtfpp_ppl_set_image_from_mem_with_options(vtfpp_ppl_handle_t handle, const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, vtfpp_image_conversion_resize_filter_e filter, float quality);
SOURCEPP_API int vtfpp_ppl_save_image_to_file(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t lod, vtfpp_image_conversion_file_format_e fileFormat);
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_bake(vtfpp_ppl_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API int vtfpp_ppl_bake_to_file(vtfpp_ppl_handle_t handle, const char* pplPath);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/PPL.h>

#endif
