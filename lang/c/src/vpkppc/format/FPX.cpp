#include <vpkppc/format/FPX.h>

#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

import vpkpp;

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fpx_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FPX::open(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fpx_open_with_options(const char* path, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FPX::open(path, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
