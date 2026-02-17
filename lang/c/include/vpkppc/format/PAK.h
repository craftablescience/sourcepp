#pragma once

#include "../PackFile.h"

SOURCEPP_EXTERNVAR const uint8_t VPKPP_PAK_FILENAME_MAX_SIZE;
SOURCEPP_EXTERNVAR const uint32_t VPKPP_PAK_SIGNATURE;

SOURCEPP_EXTERNVAR const uint8_t VPKPP_PAK_SIN_FILENAME_MAX_SIZE;
SOURCEPP_EXTERNVAR const uint32_t VPKPP_PAK_SIN_SIGNATURE;

SOURCEPP_EXTERNVAR const uint8_t VPKPP_PAK_HROT_FILENAME_MAX_SIZE;
SOURCEPP_EXTERNVAR const uint32_t VPKPP_PAK_HROT_SIGNATURE;

SOURCEPP_EXTERNVAR const char* VPKPP_PAK_EXTENSION;
SOURCEPP_EXTERNVAR const char* VPKPP_SIN_EXTENSION;

SOURCEPP_EXTERN typedef enum {
	VPKPP_PAK_TYPE_PAK,
	VPKPP_PAK_TYPE_SIN,
	VPKPP_PAK_TYPE_HROT,
} vpkpp_pak_type_e;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create_with_options(const char* path, vpkpp_pak_type_e type); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pak_type_e vpkpp_pak_get_type(vpkpp_pack_file_handle_t handle);
SOURCEPP_API void vpkpp_pak_set_type(vpkpp_pack_file_handle_t handle, vpkpp_pak_type_e type);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/PAK.h>

namespace sourceppc::convert {

inline vpkpp::PAK::Type cast(vpkpp_pak_type_e value) {
	switch (value) {
		case VPKPP_PAK_TYPE_PAK:  return vpkpp::PAK::Type::PAK;
		case VPKPP_PAK_TYPE_SIN:  return vpkpp::PAK::Type::SIN;
		case VPKPP_PAK_TYPE_HROT: return vpkpp::PAK::Type::HROT;
	}
	return vpkpp::PAK::Type::PAK;
}

inline vpkpp_pak_type_e cast(vpkpp::PAK::Type value) {
	switch (value) {
		case vpkpp::PAK::Type::PAK:  return VPKPP_PAK_TYPE_PAK;
		case vpkpp::PAK::Type::SIN:  return VPKPP_PAK_TYPE_SIN;
		case vpkpp::PAK::Type::HROT: return VPKPP_PAK_TYPE_HROT;
	}
	return VPKPP_PAK_TYPE_PAK;
}

} // namespace sourceppc::convert

#endif
