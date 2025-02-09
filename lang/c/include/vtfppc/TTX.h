#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "VTF.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* vtfpp_ttx_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vtfpp_ttx_close
SOURCEPP_API vtfpp_ttx_handle_t vtfpp_ttx_create(vtfpp_vtf_handle_t vtf);

// REQUIRES MANUAL FREE: vtfpp_ttx_close
SOURCEPP_API vtfpp_ttx_handle_t vtfpp_ttx_open_from_mem(const unsigned char* tthData, size_t tthLen, const unsigned char* ttzData, size_t ttzLen);

// REQUIRES MANUAL FREE: vtfpp_ttx_close
SOURCEPP_API vtfpp_ttx_handle_t vtfpp_ttx_open_from_file(const char* tthPath, const char* ttzPath);

SOURCEPP_API int vtfpp_ttx_is_valid(vtfpp_ttx_handle_t handle);

SOURCEPP_API uint8_t vtfpp_ttx_get_major_version(vtfpp_ttx_handle_t handle);

SOURCEPP_API uint8_t vtfpp_ttx_get_minor_version(vtfpp_ttx_handle_t handle);

SOURCEPP_API void vtfpp_ttx_set_version(vtfpp_ttx_handle_t handle, uint8_t majorVersion, uint8_t minorVersion);

SOURCEPP_API void vtfpp_ttx_set_major_version(vtfpp_ttx_handle_t handle, uint8_t majorVersion);

SOURCEPP_API void vtfpp_ttx_set_minor_version(vtfpp_ttx_handle_t handle, uint8_t minorVersion);

SOURCEPP_API uint8_t vtfpp_ttx_get_aspect_ratio_type(vtfpp_ttx_handle_t handle);

SOURCEPP_API void vtfpp_ttx_set_aspect_ratio_type(vtfpp_ttx_handle_t handle, uint8_t aspectRatioType);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_uint64_t vtfpp_ttx_get_mip_flags(vtfpp_ttx_handle_t handle);

SOURCEPP_API void vtfpp_ttx_set_mip_flags(vtfpp_ttx_handle_t handle, const uint64_t* mipFlags, size_t mipFlagsLen);

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_ttx_get_vtf(vtfpp_ttx_handle_t handle);

SOURCEPP_API void vtfpp_ttx_set_vtf(vtfpp_ttx_handle_t handle, vtfpp_vtf_handle_t vtf);

SOURCEPP_API int16_t vtfpp_ttx_get_compression_level(vtfpp_ttx_handle_t handle);

SOURCEPP_API void vtfpp_ttx_set_compression_level(vtfpp_ttx_handle_t handle, int16_t compressionLevel);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ttx_bake(vtfpp_ttx_handle_t handle, size_t* tthLen, size_t* ttzLen);

SOURCEPP_API int vtfpp_ttx_bake_to_file(vtfpp_ttx_handle_t handle, const char* tthPath, const char* ttzPath);

SOURCEPP_API void vtfpp_ttx_close(vtfpp_ttx_handle_t* handle);
