#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create_with_options(const char* path, int hrot);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open(const char* path, vpkpp_entry_callback_t callback);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_pak_guid(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_pak_is_hrot(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_pak_set_hrot(vpkpp_pack_file_handle_t handle, int hrot);
