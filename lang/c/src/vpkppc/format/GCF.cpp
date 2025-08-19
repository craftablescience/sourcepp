#include <vpkppc/format/GCF.h>

#include <vpkpp/format/GCF.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gcf_open(const char* path, vpkpp_entry_callback_t callback, vpkpp_pack_file_open_property_request_t requestProperty) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GCF::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr), requestProperty ? [requestProperty](PackFile* packFile_, PackFile::OpenProperty property) {
		return Convert::fromBuffer<std::byte>(requestProperty(packFile_, static_cast<vpkpp_pack_file_open_property_e>(property)));
	} : static_cast<PackFile::OpenPropertyRequest>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_gcf_guid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(GCF::GUID);
}

SOURCEPP_API uint32_t vpkpp_gcf_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return dynamic_cast<GCF*>(Convert::packFile(handle))->getVersion();
}

SOURCEPP_API uint32_t vpkpp_gcf_get_appid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return dynamic_cast<GCF*>(Convert::packFile(handle))->getAppID();
}

SOURCEPP_API uint32_t vpkpp_gcf_get_app_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return dynamic_cast<GCF*>(Convert::packFile(handle))->getAppVersion();
}
