#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const uint32_t VPKPP_PCK_SIGNATURE;
SOURCEPP_EXTERNVAR const char* VPKPP_PCK_PATH_PREFIX;
SOURCEPP_EXTERNVAR const char* VPKPP_PCK_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pck_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pck_create_with_options(const char* path, uint32_t version, uint32_t godotMajorVersion, uint32_t godotMinorVersion, uint32_t godotPatchVersion); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pck_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/PCK.h>

#endif
