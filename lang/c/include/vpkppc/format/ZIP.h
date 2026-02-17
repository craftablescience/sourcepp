#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const char* VPKPP_BEE_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_BMZ_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_FPK_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_PK3_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_PK4_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_PKZ_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_ZIP_EXTENSION;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_zip_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_zip_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_entry_compression_type_e vpkpp_zip_get_entry_compression_type(vpkpp_pack_file_handle_t handle, const char* path);
SOURCEPP_API void vpkpp_zip_set_entry_compression_type(vpkpp_pack_file_handle_t handle, const char* path, vpkpp_entry_compression_type_e type);
SOURCEPP_API int16_t vpkpp_zip_get_entry_compression_strength(vpkpp_pack_file_handle_t handle, const char* path);
SOURCEPP_API void vpkpp_zip_set_entry_compression_strength(vpkpp_pack_file_handle_t handle, const char* path, int16_t strength);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/ZIP.h>

#endif
