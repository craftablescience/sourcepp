#include <vpkppc/format/PCK.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_PCK_SIGNATURE = PCK_SIGNATURE;
const char* VPKPP_PCK_PATH_PREFIX  = PCK_PATH_PREFIX.data();
const char* VPKPP_PCK_EXTENSION    = PCK_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pck_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PCK::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pck_create_with_options(const char* path, uint32_t version, uint32_t godotMajorVersion, uint32_t godotMinorVersion, uint32_t godotPatchVersion) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PCK::create(path, version, godotMajorVersion, godotMinorVersion, godotPatchVersion);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pck_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PCK::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
