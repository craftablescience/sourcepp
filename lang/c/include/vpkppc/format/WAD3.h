#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const int8_t VPKPP_WAD3_FILENAME_MAX_SIZE;
SOURCEPP_EXTERNVAR const uint32_t VPKPP_WAD3_SIGNATURE;
SOURCEPP_EXTERNVAR const std::string_view VPKPP_WAD3_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_wad3_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_wad3_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/WAD3.h>

#endif
