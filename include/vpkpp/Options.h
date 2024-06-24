#pragma once

#include <sourcepp/math/Integer.h>

namespace vpkpp {

/// VPK - Maximum preload data size in bytes
constexpr uint32_t VPK_MAX_PRELOAD_BYTES = 1024;

/// VPK - Chunk size in bytes (default is 200mb)
constexpr uint32_t VPK_DEFAULT_CHUNK_SIZE = 200 * 1024 * 1024;

struct PackFileOptions {
	/// GMA - Write CRCs for files and the overall GMA file when baking
	bool gma_writeCRCs = true;

	/// VPK - Version (ignored when opening an existing VPK)
	uint32_t vpk_version = 2;

	/// VPK - If this value is 0, chunks have an unlimited size (not controlled by the library)
	/// Chunk size is max 4gb, but since its not useful to have large chunks, try to keep the
	/// preferred chunk size around the default
	uint32_t vpk_preferredChunkSize = VPK_DEFAULT_CHUNK_SIZE;

	/// VPK - Controls generation of per-file MD5 hashes (only for VPK v2)
	bool vpk_generateMD5Entries = false;

	/// ZIP/BSP - The compression method. Check the MZ_COMPRESS_METHOD definitions for valid values. Only accepts STORE currently
	uint16_t zip_compressionMethod = 0; // MZ_COMPRESS_METHOD_STORE
};

struct EntryOptions {
	/// VPK - Save this entry to the directory VPK
	bool vpk_saveToDirectory = false;

	/// VPK - The amount in bytes of the file to preload. Maximum is controlled by VPK_MAX_PRELOAD_BYTES
	uint32_t vpk_preloadBytes = 0;
};

} // namespace vpkpp
