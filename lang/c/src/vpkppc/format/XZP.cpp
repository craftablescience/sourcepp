#include <vpkppc/format/XZP.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_XZP_HEADER_SIGNATURE = XZP_HEADER_SIGNATURE;
const uint32_t VPKPP_XZP_FOOTER_SIGNATURE = XZP_FOOTER_SIGNATURE;
const char* VPKPP_XZP_EXTENSION           = XZP_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_xzp_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = XZP::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
