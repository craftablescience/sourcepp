#include <vpkppc/format/OO7.h>

#include <vpkpp/format/OO7.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_007_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = OO7::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
