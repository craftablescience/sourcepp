#include <vpkppc/format/VPP.h>

#include <vpkpp/format/VPP.h>

#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace sourceppc;
using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_vpp_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = VPP::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
