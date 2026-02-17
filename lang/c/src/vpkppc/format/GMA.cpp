#include <vpkppc/format/GMA.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_GMA_SIGNATURE = GMA_SIGNATURE;
const char* VPKPP_GMA_EXTENSION    = GMA_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gma_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GMA::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
