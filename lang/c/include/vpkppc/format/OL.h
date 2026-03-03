#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const char* VPKPP_OL_SIGNATURE;
VPKPP_EXTERNVAR const char* VPKPP_OL_EXTENSION;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_ol_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API sourcepp_string_t vpkpp_ol_get_notes(vpkpp_pack_file_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_string_free
VPKPP_API sourcepp_string_t vpkpp_ol_get_entry_notes(vpkpp_pack_file_handle_t handle, const char* path); // REQUIRES MANUAL FREE: sourcepp_string_free

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/OL.h>

#endif
