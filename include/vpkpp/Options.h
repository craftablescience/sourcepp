#pragma once

#include <sourcepp/Math.h>

namespace vpkpp {

enum class EntryCompressionType : int16_t {
	NO_OVERRIDE = -1,

	// Matches MZ_COMPRESS_METHOD_<type> defines
	NO_COMPRESS =  0,
	DEFLATE     =  8,
	BZIP2       = 12,
	LZMA        = 14,
	ZSTD        = 93,
	XZ          = 95,
};

struct BakeOptions {
	/// BSP/ZIP - Override compression type of all stored entries
	EntryCompressionType zip_compressionTypeOverride = EntryCompressionType::NO_OVERRIDE;

	/// BSP/VPK/ZIP - Compression strength
	int16_t zip_compressionStrength = 5;

	/// GMA - Write CRCs for files and the overall GMA file when baking
	bool gma_writeCRCs = true;

	/// VPK - Generate MD5 hashes for each file (VPK v2 only)
	bool vpk_generateMD5Entries = false;
};

struct EntryOptions {
	/// BSP/ZIP - The compression format
	EntryCompressionType zip_compressionType = EntryCompressionType::NO_COMPRESS;

	/// BSP/ZIP - The compression strength
	int16_t zip_compressionStrength = 5;

	/// VPK - Save this entry to the directory VPK
	bool vpk_saveToDirectory = false;

	/// VPK - The amount in bytes of the file to preload. Maximum is controlled by VPK_MAX_PRELOAD_BYTES (format/VPK.h)
	uint32_t vpk_preloadBytes = 0;
};

} // namespace vpkpp
