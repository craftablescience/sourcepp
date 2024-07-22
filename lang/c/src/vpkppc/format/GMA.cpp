#include <vpkppc/format/GMA.h>

#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

import vpkpp;

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gma_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GMA::open(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gma_open_with_options(const char* path, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GMA::open(path, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
