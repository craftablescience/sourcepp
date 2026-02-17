#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

typedef void* vpkpp_entry_handle_t;

SOURCEPP_API uint32_t vpkpp_entry_get_flags(vpkpp_entry_handle_t handle);
SOURCEPP_API uint32_t vpkpp_entry_get_archive_index(vpkpp_entry_handle_t handle);
SOURCEPP_API uint64_t vpkpp_entry_get_length(vpkpp_entry_handle_t handle);
SOURCEPP_API uint64_t vpkpp_entry_get_compressed_length(vpkpp_entry_handle_t handle);
SOURCEPP_API uint64_t vpkpp_entry_get_offset(vpkpp_entry_handle_t handle);
SOURCEPP_API sourcepp_buffer_t vpkpp_entry_get_extra_data(vpkpp_entry_handle_t handle);
SOURCEPP_API uint32_t vpkpp_entry_get_crc32(vpkpp_entry_handle_t handle);
SOURCEPP_API int vpkpp_entry_is_unbaked(vpkpp_entry_handle_t handle);
SOURCEPP_API void vpkpp_entry_close(vpkpp_entry_handle_t* handle);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/Entry.h>

#endif
