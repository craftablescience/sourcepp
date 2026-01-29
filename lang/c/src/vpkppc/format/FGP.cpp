#include <vpkppc/format/FGP.h>

#include <vpkpp/format/FGP.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_create(const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FGP::create(path);
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_fgp_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = FGP::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}
