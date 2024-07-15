#include <vpkppc/Convert.hpp>

#include <vpkpp/PackFile.h>

using namespace vpkpp;

PackFile* Convert::packFile(vpkpp_pack_file_handle_t handle) {
	return static_cast<PackFile*>(handle);
}

Entry* Convert::entry(vpkpp_entry_handle_t handle) {
	return static_cast<Entry*>(handle);
}

PackFileOptions Convert::optionsFromC(vpkpp_pack_file_options_t options) {
	return {
		.gma_writeCRCs = options.gma_writeCRCs,
		.vpk_version = options.vpk_version,
		.vpk_preferredChunkSize = options.vpk_preferredChunkSize,
		.vpk_generateMD5Entries = options.vpk_generateMD5Entries,
		.zip_compressionMethod = options.zip_compressionMethod,
	};
}

vpkpp_pack_file_options_t Convert::optionsToC(PackFileOptions options) {
	return {
		.gma_writeCRCs = options.gma_writeCRCs,
		.vpk_version = options.vpk_version,
		.vpk_preferredChunkSize = options.vpk_preferredChunkSize,
		.vpk_generateMD5Entries = options.vpk_generateMD5Entries,
		.zip_compressionMethod = options.zip_compressionMethod,
	};
}
