#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const uint32_t VPKPP_FGP_SIGNATURE;
SOURCEPP_EXTERNVAR const char* VPKPP_FGP_EXTENSION;

SOURCEPP_EXTERNVAR const char* VPKPP_FGP_HASHED_FILEPATH_PREFIX;
SOURCEPP_EXTERNVAR const uint64_t VPKPP_FGP_SOURCEPP_FILENAMES_SIGNATURE;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API sourcepp_string_t vpkpp_fgp_get_loading_screen_file_path(vpkpp_pack_file_handle_t handle);
SOURCEPP_API void vpkpp_fgp_set_loading_screen_file_path(vpkpp_pack_file_handle_t handle, const char* path);
SOURCEPP_API uint32_t vpkpp_fgp_hash_file_path(const char* filepath);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/FGP.h>

#endif
