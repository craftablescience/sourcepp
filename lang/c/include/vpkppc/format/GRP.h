#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR int8_t VPKPP_GRP_FILENAME_MAX_SIZE;
VPKPP_EXTERNVAR const char* VPKPP_GRP_SIGNATURE;
VPKPP_EXTERNVAR const char* VPKPP_GRP_EXTENSION;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_grp_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_grp_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/GRP.h>

#endif
