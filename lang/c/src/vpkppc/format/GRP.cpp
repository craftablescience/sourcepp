#include <vpkppc/format/GRP.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

int8_t VPKPP_GRP_FILENAME_MAX_SIZE = GRP_FILENAME_MAX_SIZE;
const char* VPKPP_GRP_SIGNATURE    = GRP_SIGNATURE.data();
const char* VPKPP_GRP_EXTENSION    = GRP_EXTENSION.data();

VPKPP_API vpkpp_pack_file_handle_t vpkpp_grp_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GRP::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API vpkpp_pack_file_handle_t vpkpp_grp_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GRP::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
