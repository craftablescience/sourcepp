#pragma once

#include "../PackFile.h"

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_create_with_options(const char* path, uint32_t version);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpk_open(const char* path, vpkpp_entry_callback_t callback);

SOURCEPP_API bool vpkpp_vpk_generate_keypair_files(const char* path);

SOURCEPP_API bool vpkpp_vpk_sign_from_file(vpkpp_pack_file_handle_t handle, const char* filename);

SOURCEPP_API bool vpkpp_vpk_sign_from_mem(vpkpp_pack_file_handle_t handle, const unsigned char* privateKeyBuffer, size_t privateKeyLen, const unsigned char* publicKeyBuffer, size_t publicKeyLen);

SOURCEPP_API uint32_t vpkpp_vpk_get_version(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_vpk_set_version(vpkpp_pack_file_handle_t handle, uint32_t version);

SOURCEPP_API uint32_t vpkpp_vpk_get_chunk_size(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_vpk_set_chunk_size(vpkpp_pack_file_handle_t handle, uint32_t chunkSize);
