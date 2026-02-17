#include <vpkppc/format/GCF.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_GCF_EXTENSION = GCF_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_gcf_open(const char* path, vpkpp_entry_callback_t callback, vpkpp_pack_file_open_property_request_t requestProperty) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = GCF::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr), requestProperty ? [requestProperty](PackFile* packFile_, PackFile::OpenProperty property) {
		return convert::fromBuffer<std::byte>(requestProperty(packFile_, static_cast<vpkpp_pack_file_open_property_e>(property)));
	} : static_cast<PackFile::OpenPropertyRequest>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API uint32_t vpkpp_gcf_get_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	const auto* gcf = dynamic_cast<GCF*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(gcf, 0);

	return gcf->getVersion();
}

SOURCEPP_API uint32_t vpkpp_gcf_get_appid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	const auto* gcf = dynamic_cast<GCF*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(gcf, 0);

	return gcf->getAppID();
}

SOURCEPP_API uint32_t vpkpp_gcf_get_app_version(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	const auto* gcf = dynamic_cast<GCF*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(gcf, 0);

	return gcf->getAppVersion();
}
