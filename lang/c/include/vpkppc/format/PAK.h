#pragma once

#include "../PackFile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VPKPP_PAK_TYPE_PAK,
	VPKPP_PAK_TYPE_SIN,
	VPKPP_PAK_TYPE_HROT,
} vpkpp_pak_type_e;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create_with_options(const char* path, vpkpp_pak_type_e type);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open(const char* path, vpkpp_entry_callback_t callback);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_pak_guid(vpkpp_pack_file_handle_t handle);

SOURCEPP_API vpkpp_pak_type_e vpkpp_pak_get_type(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_pak_set_type(vpkpp_pack_file_handle_t handle, vpkpp_pak_type_e type);
