#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_ol_open(const char* path, vpkpp_entry_callback_t callback);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_ol_get_notes(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_ol_get_entry_notes(vpkpp_pack_file_handle_t handle, const char* path);
