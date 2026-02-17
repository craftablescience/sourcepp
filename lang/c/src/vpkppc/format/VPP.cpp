#include <vpkppc/format/VPP.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const uint32_t VPKPP_VPP_SIGNATURE_LIL = VPP_SIGNATURE_LIL;
const uint32_t VPKPP_VPP_SIGNATURE_BIG = VPP_SIGNATURE_BIG;
const uint32_t VPKPP_VPP_ALIGNMENT     = VPP_ALIGNMENT;
const char* VPKPP_VPP_EXTENSION        = VPP_EXTENSION.data();
const char* VPKPP_VPP_EXTENSION_PC     = VPP_EXTENSION_PC.data();
const char* VPKPP_VPP_EXTENSION_XBOX2  = VPP_EXTENSION_XBOX2.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpp_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPP::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
