#include <vpkppc/format/REZ.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_REZ_EXTENSION = REZ_EXTENSION.data();

VPKPP_API vpkpp_pack_file_handle_t vpkpp_rez_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = REZ::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

VPKPP_API sourcepp_string_t vpkpp_rez_get_file_type(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	const auto* rez = dynamic_cast<REZ*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(rez, SOURCEPP_STRING_INVALID);

	return convert::toString(rez->getFileType());
}

VPKPP_API sourcepp_string_t vpkpp_rez_get_user_title(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	const auto* rez = dynamic_cast<REZ*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(rez, SOURCEPP_STRING_INVALID);

	return convert::toString(rez->getUserTitle());
}

VPKPP_API uint32_t vpkpp_rez_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	const auto* rez = dynamic_cast<REZ*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(rez, 0);

	return rez->getVersion();
}
