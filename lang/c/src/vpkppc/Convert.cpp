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
		.zip_compressionTypeOverride = static_cast<EntryCompressionType>(options.zip_compressionTypeOverride),
		.zip_compressionStrength = options.zip_compressionStrength,
		.gma_writeCRCs = static_cast<bool>(options.gma_writeCRCs),
		.vpk_generateMD5Entries = static_cast<bool>(options.vpk_generateMD5Entries),
		.vpk_useBuggyExtensionHandling = static_cast<bool>(options.vpk_useBuggyExtensionHandling),
	};
}

EntryOptions Convert::optionsFromC(vpkpp_entry_options_t options) {
	return {
		.zip_compressionType = static_cast<EntryCompressionType>(options.zip_compressionType),
		.zip_compressionStrength = options.zip_compressionStrength,
		.vpk_preloadBytes = options.vpk_preloadBytes,
		.vpk_saveToDirectory = static_cast<bool>(options.vpk_saveToDirectory),
	};
}

vpkpp_bake_options_t Convert::optionsToC(BakeOptions options) {
	return {
		.zip_compressionTypeOverride = static_cast<int16_t>(options.zip_compressionTypeOverride),
		.zip_compressionStrength = options.zip_compressionStrength,
		.gma_writeCRCs = options.gma_writeCRCs,
		.vpk_generateMD5Entries = options.vpk_generateMD5Entries,
		.vpk_useBuggyExtensionHandling = options.vpk_useBuggyExtensionHandling,
	};
}

vpkpp_entry_options_t Convert::optionsToC(EntryOptions options) {
	return {
		.zip_compressionType = static_cast<int16_t>(options.zip_compressionType),
		.zip_compressionStrength = options.zip_compressionStrength,
		.vpk_preloadBytes = options.vpk_preloadBytes,
		.vpk_saveToDirectory = options.vpk_saveToDirectory,
	};
}
