#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const uint32_t VPKPP_APK_SIGNATURE;
VPKPP_EXTERNVAR const char* VPKPP_APK_EXTENSION;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_apk_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_apk_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/APK.h>

#endif
