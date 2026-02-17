#include <vpkppc/format/PAK.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint8_t VPKPP_PAK_FILENAME_MAX_SIZE = PAK_FILENAME_MAX_SIZE;
const uint32_t VPKPP_PAK_SIGNATURE        = PAK_SIGNATURE;

const uint8_t VPKPP_PAK_SIN_FILENAME_MAX_SIZE = PAK_SIN_FILENAME_MAX_SIZE;
const uint32_t VPKPP_PAK_SIN_SIGNATURE        = PAK_SIN_SIGNATURE;

const uint8_t VPKPP_PAK_HROT_FILENAME_MAX_SIZE = PAK_HROT_FILENAME_MAX_SIZE;
const uint32_t VPKPP_PAK_HROT_SIGNATURE        = PAK_HROT_SIGNATURE;

const char* VPKPP_PAK_EXTENSION = PAK_EXTENSION.data();
const char* VPKPP_SIN_EXTENSION = SIN_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PAK::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create_with_options(const char* path, vpkpp_pak_type_e type) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PAK::create(path, convert::cast(type));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PAK::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pak_type_e vpkpp_pak_get_type(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_PAK_TYPE_PAK);

	const auto* pak = dynamic_cast<PAK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(pak, VPKPP_PAK_TYPE_PAK);

	return convert::cast(pak->getType());
}

SOURCEPP_API void vpkpp_pak_set_type(vpkpp_pack_file_handle_t handle, vpkpp_pak_type_e type) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* pak = dynamic_cast<PAK*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN(pak);

	return pak->setType(convert::cast(type));
}
