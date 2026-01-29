#include <vpkppc/format/OL.h>

#include <vpkpp/format/OL.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_ol_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = OL::open(path, callback ? [callback](const std::string& path, const Entry& entry) {
		callback(path.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_ol_guid(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(OL::GUID);
}

SOURCEPP_API sourcepp_string_t vpkpp_ol_get_notes(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	auto* ol = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN_VAL(ol->isInstanceOf<OL>(), SOURCEPP_STRING_INVALID);

	return convert::toString(dynamic_cast<OL*>(Convert::packFile(handle))->getNotes());
}

SOURCEPP_API sourcepp_string_t vpkpp_ol_get_entry_notes(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	auto* ol = Convert::packFile(handle);
	SOURCEPP_EARLY_RETURN_VAL(ol->isInstanceOf<OL>(), SOURCEPP_STRING_INVALID);

	const auto notes = dynamic_cast<OL*>(Convert::packFile(handle))->getEntryNotes(path);
	return convert::toString(notes ? *notes : "");
}
