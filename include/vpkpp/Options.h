#pragma once

#include <sourcepp/math/Integer.h>

namespace vpkpp {

struct BakeOptions {
	/// GMA - Write CRCs for files and the overall GMA file when baking
	bool gma_writeCRCs = true;

	/// VPK - Generate MD5 hashes for each file (VPK v2 only)
	bool vpk_generateMD5Entries = false;
};

struct EntryOptions {
	/// VPK - Save this entry to the directory VPK
	bool vpk_saveToDirectory = false;

	/// VPK - The amount in bytes of the file to preload. Maximum is controlled by VPK_MAX_PRELOAD_BYTES (format/VPK.h)
	uint32_t vpk_preloadBytes = 0;
};

} // namespace vpkpp
