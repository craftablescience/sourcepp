#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const char* VPKPP_TAB_EXTENSION;

VPKPP_EXTERNVAR const int VPKPP_TAB_FILENAME_MAX_SIZE;
VPKPP_EXTERNVAR const char* VPKPP_TAB_HASHED_FILEPATH_PREFIX;

VPKPP_EXTERNVAR const char* VPKPP_ARC_EXTENSION;
VPKPP_EXTERNVAR const uint32_t VPKPP_ARC_CHUNK_SIZE;

VPKPP_EXTERN typedef enum {
	VPKPP_TAB_VERSION_JC1_LE,
	VPKPP_TAB_VERSION_JC1_BE,
} vpkpp_tab_version_e;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_create_with_options(const char* path, vpkpp_tab_version_e version, uint32_t sectorSize); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_tab_version_e vpkpp_tab_get_version(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_tab_set_version(vpkpp_pack_file_handle_t handle, vpkpp_tab_version_e version);
VPKPP_API uint32_t vpkpp_tab_get_sector_size(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_tab_set_sector_size(vpkpp_pack_file_handle_t handle, uint32_t sectorSize);
VPKPP_API uint32_t vpkpp_tab_hash_file_path(const char* filepath);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/TAB.h>

namespace sourceppc::convert {

inline vpkpp::TAB::Version cast(vpkpp_tab_version_e value) {
	switch (value) {
		case VPKPP_TAB_VERSION_JC1_LE: return vpkpp::TAB::Version::JC1_LE;
		case VPKPP_TAB_VERSION_JC1_BE: return vpkpp::TAB::Version::JC1_BE;
	}
	return vpkpp::TAB::Version::JC1_LE;
}

inline vpkpp_tab_version_e cast(vpkpp::TAB::Version value) {
	switch (value) {
		case vpkpp::TAB::Version::JC1_LE: return VPKPP_TAB_VERSION_JC1_LE;
		case vpkpp::TAB::Version::JC1_BE: return VPKPP_TAB_VERSION_JC1_BE;
	}
	return VPKPP_TAB_VERSION_JC1_LE;
}

} // namespace sourceppc::convert

#endif
