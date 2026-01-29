#include <vpkppc/format/WAD3.h>

#include <vpkpp/format/WAD3.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_wad3_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = WAD3::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_wad3_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = WAD3::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
