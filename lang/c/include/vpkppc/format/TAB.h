#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const char* VPKPP_TAB_EXTENSION;

VPKPP_EXTERNVAR const int VPKPP_TAB_FILENAME_MAX_SIZE;
VPKPP_EXTERNVAR const char* VPKPP_TAB_HASHED_FILEPATH_PREFIX;

VPKPP_EXTERNVAR const char* VPKPP_ARC_EXTENSION;
VPKPP_EXTERNVAR const uint32_t VPKPP_ARC_CHUNK_SIZE;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API uint32_t vpkpp_tab_hash_file_path(const char* filepath);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/TAB.h>

#endif
