#include <vpkppc/format/HOG.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_HOG_SIGNATURE = HOG_SIGNATURE.data();
const char* VPKPP_HOG_EXTENSION = HOG_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_hog_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = HOG::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
