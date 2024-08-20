#include <vpkppc/format/FPX.h>

#include <vpkpp/format/FPX.h>

#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fpx_open(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FPX::open(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fpx_open_with_callback(const char* path, EntryCallback callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(callback, nullptr);

	auto packFile = FPX::open(path, {}, [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	});
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

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fpx_open_with_options_and_callback(const char* path, vpkpp_pack_file_options_t options, EntryCallback callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(callback, nullptr);

	auto packFile = FPX::open(path, Convert::optionsFromC(options), [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	});
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
