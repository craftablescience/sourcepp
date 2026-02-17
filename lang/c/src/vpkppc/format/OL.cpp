#include <vpkppc/format/OL.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

const char* VPKPP_OL_SIGNATURE = OL_SIGNATURE.data();
const char* VPKPP_OL_EXTENSION = OL_EXTENSION.data();

SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_ol_open(const char* path, vpkpp_entry_callback_t callback) {
	SOURCEPP_EARLY_RETURN_VAL(path, nullptr);

	auto packFile = OL::open(path, callback ? [callback](const std::string& entryPath, const Entry& entry) {
		callback(entryPath.c_str(), const_cast<Entry*>(&entry));
	} : static_cast<PackFile::EntryCallback>(nullptr));
	if (!packFile) {
		return nullptr;
	}
	return packFile.release();
}

SOURCEPP_API sourcepp_string_t vpkpp_ol_get_notes(vpkpp_pack_file_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	const auto* ol = dynamic_cast<OL*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(ol, SOURCEPP_STRING_INVALID);

	return convert::toString(ol->getNotes());
}

SOURCEPP_API sourcepp_string_t vpkpp_ol_get_entry_notes(vpkpp_pack_file_handle_t handle, const char* path) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(path, SOURCEPP_STRING_INVALID);

	const auto* ol = dynamic_cast<OL*>(convert::handle<PackFile>(handle));
	SOURCEPP_EARLY_RETURN_VAL(ol, SOURCEPP_STRING_INVALID);

	const auto notes = ol->getEntryNotes(path);
	return convert::toString(notes ? *notes : "");
}
