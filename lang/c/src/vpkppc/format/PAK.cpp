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

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_pak_create_with_options(const char* path, int hrot) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = PAK::create(path, hrot);
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

SOURCEPP_API int vpkpp_pak_is_hrot(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	auto* pak = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN_VAL(pak->isInstanceOf<PAK>(), false);

	return dynamic_cast<PAK*>(pak)->isHROT();
}

SOURCEPP_API void vpkpp_pak_set_hrot(vpkpp_pack_file_handle_t handle, int hrot) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* pak = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN(pak->isInstanceOf<PAK>());

	return dynamic_cast<PAK*>(pak)->setHROT(hrot);
}
