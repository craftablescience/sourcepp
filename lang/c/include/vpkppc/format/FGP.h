#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const uint32_t VPKPP_FGP_SIGNATURE;
VPKPP_EXTERNVAR const char* VPKPP_FGP_EXTENSION;

VPKPP_EXTERNVAR const char* VPKPP_FGP_HASHED_FILEPATH_PREFIX;
VPKPP_EXTERNVAR const uint64_t VPKPP_FGP_SOURCEPP_FILENAMES_SIGNATURE;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_fgp_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_fgp_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API sourcepp_string_t vpkpp_fgp_get_loading_screen_file_path(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_fgp_set_loading_screen_file_path(vpkpp_pack_file_handle_t handle, const char* path);
VPKPP_API uint32_t vpkpp_fgp_hash_file_path(const char* filepath);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/FGP.h>

#endif
