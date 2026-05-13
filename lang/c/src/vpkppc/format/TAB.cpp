#include <vpkppc/format/TAB.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_TAB_EXTENSION = TAB_EXTENSION.data();

const int VPKPP_TAB_FILENAME_MAX_SIZE = TAB_FILENAME_MAX_SIZE;
const char* VPKPP_TAB_HASHED_FILEPATH_PREFIX = TAB_HASHED_FILEPATH_PREFIX.data();

const char* VPKPP_ARC_EXTENSION = ARC_EXTENSION.data();
const uint32_t VPKPP_ARC_CHUNK_SIZE = ARC_CHUNK_SIZE;

VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = TAB::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API vpkpp_pack_file_handle_t vpkpp_tab_create_with_options(const char* path, vpkpp_tab_version_e version, uint32_t sectorSize) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = TAB::create(path, convert::cast(version), sectorSize);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

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

VPKPP_API vpkpp_tab_version_e vpkpp_tab_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_TAB_VERSION_JC1_LE);

	return convert::cast(convert::handle<TAB>(handle)->getVersion());
}

VPKPP_API void vpkpp_tab_set_version(vpkpp_pack_file_handle_t handle, vpkpp_tab_version_e version) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TAB>(handle)->setVersion(convert::cast(version));
}

VPKPP_API uint32_t vpkpp_tab_get_sector_size(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<TAB>(handle)->getSectorSize();
}

VPKPP_API void vpkpp_tab_set_sector_size(vpkpp_pack_file_handle_t handle, uint32_t sectorSize) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<TAB>(handle)->setSectorSize(sectorSize);
}

VPKPP_API uint32_t vpkpp_tab_hash_file_path(const char* filepath) {
	SOURCEPP_EARLY_RETURN_VAL(filepath, 0);

	return TAB::hashFilePath(filepath);
}
