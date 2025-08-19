#include <vpkppc/format/PAK.h>

#include <vpkpp/format/PAK.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

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

	auto packFile = PAK::create(path, static_cast<PAK::Type>(type));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PAK::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_pak_guid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(PAK::GUID);
}

SOURCEPP_API vpkpp_pak_type_e vpkpp_pak_get_type(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VPKPP_PAK_TYPE_PAK);

	auto* pak = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN_VAL(pak->isInstanceOf<PAK>(), VPKPP_PAK_TYPE_PAK);

	return static_cast<vpkpp_pak_type_e>(dynamic_cast<PAK*>(pak)->getType());
}

SOURCEPP_API void vpkpp_pak_set_type(vpkpp_pack_file_handle_t handle, vpkpp_pak_type_e type) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* pak = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN(pak->isInstanceOf<PAK>());

	return dynamic_cast<PAK*>(pak)->setType(static_cast<PAK::Type>(type));
}
