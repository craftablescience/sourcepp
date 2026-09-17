#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const uint32_t VPKPP_FPX_SIGNATURE;
VPKPP_EXTERNVAR const char* VPKPP_FPX_DIR_SUFFIX;
VPKPP_EXTERNVAR const char* VPKPP_FPX_EXTENSION;

VPKPP_EXTERNVAR const uint32_t VPKPP_VPK_SIGNATURE;
VPKPP_EXTERNVAR const char* VPKPP_VPK_DIR_SUFFIX;
VPKPP_EXTERNVAR const char* VPKPP_VPK_EXTENSION;

VPKPP_EXTERNVAR const uint16_t VPKPP_VPK_DIR_ARCHIVE_INDEX;

VPKPP_EXTERNVAR const uint16_t VPKPP_VPK_MAX_PRELOAD_BYTES;

VPKPP_EXTERNVAR const uint32_t VPKPP_VPK_DEFAULT_CHUNK_SIZE;

VPKPP_EXTERN typedef enum {
	VPKPP_VPK_VERSION_VALVE_V0   = 0,
	VPKPP_VPK_VERSION_VALVE_V1   = 1,
	VPKPP_VPK_VERSION_VALVE_V2   = 2,
	VPKPP_VPK_VERSION_TI_FPX_V10 = 10,
	VPKPP_VPK_VERSION_PROMOD_V54 = 54,
} vpkpp_vpk_version_e;

VPKPP_EXTERN typedef enum {
	VPKPP_VPK_SIGNATURE_TYPE_UNKNOWN             = -2,
	VPKPP_VPK_SIGNATURE_TYPE_LEGACY              = -1,
	VPKPP_VPK_SIGNATURE_TYPE_WHOLE_FILE          = 0,
	VPKPP_VPK_SIGNATURE_TYPE_WHOLE_FILE_CHECKSUM = 1,
} vpkpp_vpk_signature_type_e;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_with_options(const char* path, vpkpp_vpk_version_e version); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API void vpkpp_vpk_generate_keypair_files(const char* base, vpkpp_vpk_signature_type_e signatureType);
VPKPP_API int vpkpp_vpk_sign_from_file(vpkpp_pack_file_handle_t handle, const char* filename, vpkpp_vpk_signature_type_e signatureType);
VPKPP_API int vpkpp_vpk_sign_from_mem(vpkpp_pack_file_handle_t handle, const unsigned char* privateKeyBuffer, size_t privateKeyLen, const unsigned char* publicKeyBuffer, size_t publicKeyLen, vpkpp_vpk_signature_type_e signatureType);
VPKPP_API vpkpp_vpk_version_e vpkpp_vpk_get_version(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_vpk_set_version(vpkpp_pack_file_handle_t handle, vpkpp_vpk_version_e version);
VPKPP_API uint32_t vpkpp_vpk_get_chunk_size(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_vpk_set_chunk_size(vpkpp_pack_file_handle_t handle, uint32_t chunkSize);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/VPK.h>

namespace sourceppc::convert {

inline vpkpp::VPK::Version cast(vpkpp_vpk_version_e value) {
	switch (value) {
		case VPKPP_VPK_VERSION_VALVE_V0:   return vpkpp::VPK::Version::VALVE_V0;
		case VPKPP_VPK_VERSION_VALVE_V1:   return vpkpp::VPK::Version::VALVE_V1;
		case VPKPP_VPK_VERSION_VALVE_V2:   return vpkpp::VPK::Version::VALVE_V2;
		case VPKPP_VPK_VERSION_TI_FPX_V10: return vpkpp::VPK::Version::TI_FPX_V10;
		case VPKPP_VPK_VERSION_PROMOD_V54: return vpkpp::VPK::Version::PROMOD_V54;
	}
	return vpkpp::VPK::Version::VALVE_V0;
}

inline vpkpp_vpk_version_e cast(vpkpp::VPK::Version value) {
	switch (value) {
		case vpkpp::VPK::Version::VALVE_V0:   return VPKPP_VPK_VERSION_VALVE_V0;
		case vpkpp::VPK::Version::VALVE_V1:   return VPKPP_VPK_VERSION_VALVE_V1;
		case vpkpp::VPK::Version::VALVE_V2:   return VPKPP_VPK_VERSION_VALVE_V2;
		case vpkpp::VPK::Version::TI_FPX_V10: return VPKPP_VPK_VERSION_TI_FPX_V10;
		case vpkpp::VPK::Version::PROMOD_V54: return VPKPP_VPK_VERSION_PROMOD_V54;
	}
	return VPKPP_VPK_VERSION_VALVE_V0;
}

inline vpkpp::VPK::SignatureType cast(vpkpp_vpk_signature_type_e value) {
	switch (value) {
		case VPKPP_VPK_SIGNATURE_TYPE_UNKNOWN:             return vpkpp::VPK::SignatureType::UNKNOWN;
		case VPKPP_VPK_SIGNATURE_TYPE_LEGACY:              return vpkpp::VPK::SignatureType::LEGACY;
		case VPKPP_VPK_SIGNATURE_TYPE_WHOLE_FILE:          return vpkpp::VPK::SignatureType::WHOLE_FILE;
		case VPKPP_VPK_SIGNATURE_TYPE_WHOLE_FILE_CHECKSUM: return vpkpp::VPK::SignatureType::WHOLE_FILE_CHECKSUM;
	}
	return vpkpp::VPK::SignatureType::UNKNOWN;
}

inline vpkpp_vpk_signature_type_e cast(vpkpp::VPK::SignatureType value) {
	switch (value) {
		case vpkpp::VPK::SignatureType::UNKNOWN:             return VPKPP_VPK_SIGNATURE_TYPE_UNKNOWN;
		case vpkpp::VPK::SignatureType::LEGACY:              return VPKPP_VPK_SIGNATURE_TYPE_LEGACY;
		case vpkpp::VPK::SignatureType::WHOLE_FILE:          return VPKPP_VPK_SIGNATURE_TYPE_WHOLE_FILE;
		case vpkpp::VPK::SignatureType::WHOLE_FILE_CHECKSUM: return VPKPP_VPK_SIGNATURE_TYPE_WHOLE_FILE_CHECKSUM;
	}
	return VPKPP_VPK_SIGNATURE_TYPE_UNKNOWN;
}

} // namespace sourceppc::convert

#endif
