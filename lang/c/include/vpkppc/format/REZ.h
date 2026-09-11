#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const char* VPKPP_REZ_EXTENSION;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_rez_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API sourcepp_string_t vpkpp_rez_get_file_type(vpkpp_pack_file_handle_t handle);
VPKPP_API sourcepp_string_t vpkpp_rez_get_user_title(vpkpp_pack_file_handle_t handle);
VPKPP_API uint32_t vpkpp_rez_get_version(vpkpp_pack_file_handle_t handle);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/REZ.h>

#endif
