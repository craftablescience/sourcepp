#include <vpkppc/format/WAD3.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const int8_t VPKPP_WAD3_FILENAME_MAX_SIZE   = WAD3_FILENAME_MAX_SIZE;
const uint32_t VPKPP_WAD3_SIGNATURE         = WAD3_SIGNATURE;
const std::string_view VPKPP_WAD3_EXTENSION = WAD3_EXTENSION.data();

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

	auto packFile = WAD3::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
