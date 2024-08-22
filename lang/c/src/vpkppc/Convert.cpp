#include <vpkppc/Convert.hpp>

#include <vpkpp/PackFile.h>

using namespace vpkpp;

PackFile* Convert::packFile(vpkpp_pack_file_handle_t handle) {
	return static_cast<PackFile*>(handle);
}

Entry* Convert::entry(vpkpp_entry_handle_t handle) {
	return static_cast<Entry*>(handle);
}

BakeOptions Convert::optionsFromC(vpkpp_bake_options_t options) {
	return {
		.gma_writeCRCs = static_cast<bool>(options.gma_writeCRCs),
		.vpk_generateMD5Entries = static_cast<bool>(options.vpk_generateMD5Entries),
	};
}

EntryOptions Convert::optionsFromC(vpkpp_entry_options_t options) {
	return {
		.vpk_saveToDirectory = static_cast<bool>(options.vpk_saveToDirectory),
		.vpk_preloadBytes = options.vpk_preloadBytes,
	};
}

vpkpp_bake_options_t Convert::optionsToC(BakeOptions options) {
	return {
		.gma_writeCRCs = options.gma_writeCRCs,
		.vpk_generateMD5Entries = options.vpk_generateMD5Entries,
	};
}

vpkpp_entry_options_t Convert::optionsToC(EntryOptions options) {
	return {
		.vpk_saveToDirectory = options.vpk_saveToDirectory,
		.vpk_preloadBytes = options.vpk_preloadBytes,
	};
}
