#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_zip_create(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_zip_open(const char* path, vpkpp_entry_callback_t callback);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_zip_guid(vpkpp_pack_file_handle_t handle);

SOURCEPP_API vpkpp_entry_compression_type_e vpkpp_zip_get_entry_compression_type(vpkpp_pack_file_handle_t handle, const char* path);

SOURCEPP_API void vpkpp_zip_set_entry_compression_type(vpkpp_pack_file_handle_t handle, const char* path, vpkpp_entry_compression_type_e type);

SOURCEPP_API int16_t vpkpp_zip_get_entry_compression_strength(vpkpp_pack_file_handle_t handle, const char* path);

SOURCEPP_API void vpkpp_zip_set_entry_compression_strength(vpkpp_pack_file_handle_t handle, const char* path, int16_t strength);
