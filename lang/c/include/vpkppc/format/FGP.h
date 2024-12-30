#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_create(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_open(const char* path, vpkpp_entry_callback_t callback);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_fgp_guid(vpkpp_pack_file_handle_t handle);
