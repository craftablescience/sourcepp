#include <vpkppc/format/GRP.h>

#include <vpkpp/format/GRP.h>

#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_grp_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GRP::open(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_grp_open_with_options(const char* path, vpkpp_pack_file_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GRP::open(path, Convert::optionsFromC(options));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
