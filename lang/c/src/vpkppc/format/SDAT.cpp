#include <vpkppc/format/SDAT.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_SDAT_SIGNATURE = SDAT_SIGNATURE;
const char* VPKPP_SDAT_EXTENSION    = SDAT_EXTENSION.data();

VPKPP_API vpkpp_pack_file_handle_t vpkpp_sdat_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = SDAT::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API vpkpp_pack_file_handle_t vpkpp_sdat_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = SDAT::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
