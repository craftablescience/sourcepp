#pragma once

#include "../PackFile.h"

VPKPP_EXTERNVAR const uint32_t VPKPP_VPK_SIGNATURE;
VPKPP_EXTERNVAR const uint16_t VPKPP_VPK_DIR_INDEX;
VPKPP_EXTERNVAR const uint16_t VPKPP_VPK_ENTRY_TERM;
VPKPP_EXTERNVAR const char* VPKPP_VPK_DIR_SUFFIX;
VPKPP_EXTERNVAR const char* VPKPP_VPK_EXTENSION;
VPKPP_EXTERNVAR const char* VPKPP_VPK_KEYPAIR_PUBLIC_KEY_TEMPLATE;
VPKPP_EXTERNVAR const char* VPKPP_VPK_KEYPAIR_PRIVATE_KEY_TEMPLATE;
VPKPP_EXTERNVAR const uint16_t VPKPP_VPK_MAX_PRELOAD_BYTES;
VPKPP_EXTERNVAR const uint32_t VPKPP_VPK_DEFAULT_CHUNK_SIZE;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create(const char* path); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_with_options(const char* path, uint32_t version); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open(const char* path, vpkpp_entry_callback_t callback); // REQUIRES MANUAL FREE: vpkpp_close
VPKPP_API int vpkpp_vpk_generate_keypair_files(const char* path);
VPKPP_API int vpkpp_vpk_sign_from_file(vpkpp_pack_file_handle_t handle, const char* filename);
VPKPP_API int vpkpp_vpk_sign_from_mem(vpkpp_pack_file_handle_t handle, const unsigned char* privateKeyBuffer, size_t privateKeyLen, const unsigned char* publicKeyBuffer, size_t publicKeyLen);
VPKPP_API uint32_t vpkpp_vpk_get_version(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_vpk_set_version(vpkpp_pack_file_handle_t handle, uint32_t version);
VPKPP_API uint32_t vpkpp_vpk_get_chunk_size(vpkpp_pack_file_handle_t handle);
VPKPP_API void vpkpp_vpk_set_chunk_size(vpkpp_pack_file_handle_t handle, uint32_t chunkSize);

// C++ conversion routines
#ifdef __cplusplus

#include <vpkpp/format/VPK.h>

#endif
