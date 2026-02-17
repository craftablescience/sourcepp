#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const uint32_t VPKPP_APK_SIGNATURE;
SOURCEPP_EXTERNVAR const char* VPKPP_APK_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_apk_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_apk_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/APK.h>

#endif
