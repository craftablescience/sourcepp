#include <vpkppc/format/TAB.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_TAB_EXTENSION = TAB_EXTENSION.data();

const int VPKPP_TAB_FILENAME_MAX_SIZE = TAB_FILENAME_MAX_SIZE;
const char* VPKPP_TAB_HASHED_FILEPATH_PREFIX = TAB_HASHED_FILEPATH_PREFIX.data();

const char* VPKPP_ARC_EXTENSION = ARC_EXTENSION.data();
const uint32_t VPKPP_ARC_CHUNK_SIZE = ARC_CHUNK_SIZE;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = TAB::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API uint32_t vpkpp_tab_hash_file_path(const char* filepath) {
	return TAB::hashFilePath(filepath);
}
