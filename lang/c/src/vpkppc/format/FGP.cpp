#include <vpkppc/format/FGP.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_FGP_SIGNATURE = FGP_SIGNATURE;
const char* VPKPP_FGP_EXTENSION    = FGP_EXTENSION.data();

const char* VPKPP_FGP_HASHED_FILEPATH_PREFIX          = FGP_HASHED_FILEPATH_PREFIX.data();
const uint64_t VPKPP_FGP_SOURCEPP_FILENAMES_SIGNATURE = FGP_SOURCEPP_FILENAMES_SIGNATURE;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FGP::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FGP::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API sourcepp_string_t vpkpp_fgp_get_loading_screen_file_path(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	const auto* fgp = dynamic_cast<FGP*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(fgp, SOURCEPP_STRING_INVALID);

	return convert::toString(fgp->getLoadingScreenFilePath());
}

SOURCEPP_API void vpkpp_fgp_set_loading_screen_file_path(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(path);

	auto* fgp = dynamic_cast<FGP*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN(fgp);

	fgp->setLoadingScreenFilePath(path);
}

SOURCEPP_API uint32_t vpkpp_fgp_hash_file_path(const char* filepath) {
	return FGP::hashFilePath(filepath);
}
