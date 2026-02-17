#include <vpkppc/Entry.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vpkpp;

SOURCEPP_API uint32_t vpkpp_entry_get_flags(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<Entry>(handle)->flags;
}

SOURCEPP_API uint32_t vpkpp_entry_get_archive_index(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<Entry>(handle)->archiveIndex;
}

SOURCEPP_API uint64_t vpkpp_entry_get_length(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<Entry>(handle)->length;
}

SOURCEPP_API uint64_t vpkpp_entry_get_compressed_length(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<Entry>(handle)->compressedLength;
}

SOURCEPP_API uint64_t vpkpp_entry_get_offset(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<Entry>(handle)->offset;
}

SOURCEPP_API sourcepp_buffer_t vpkpp_entry_get_extra_data(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<Entry>(handle)->extraData);
}

SOURCEPP_API uint32_t vpkpp_entry_get_crc32(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<Entry>(handle)->crc32;
}

SOURCEPP_API int vpkpp_entry_is_unbaked(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<Entry>(handle)->unbaked;
}

SOURCEPP_API void vpkpp_entry_close(vpkpp_entry_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<Entry>(*handle);
	*handle = nullptr;
}
