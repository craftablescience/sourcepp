#include <vpkppc/Entry.h>

#include <vpkpp/Entry.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API uint32_t vpkpp_entry_get_flags(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::entry(handle)->flags;
}

SOURCEPP_API uint32_t vpkpp_entry_get_archive_index(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::entry(handle)->archiveIndex;
}

SOURCEPP_API uint64_t vpkpp_entry_get_length(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::entry(handle)->length;
}

SOURCEPP_API uint64_t vpkpp_entry_get_compressed_length(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::entry(handle)->compressedLength;
}

SOURCEPP_API uint64_t vpkpp_entry_get_offset(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::entry(handle)->offset;
}

SOURCEPP_API sourcepp_buffer_t vpkpp_entry_get_extra_data(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::entry(handle)->extraData);
}

SOURCEPP_API uint32_t vpkpp_entry_get_crc32(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::entry(handle)->crc32;
}

SOURCEPP_API int vpkpp_entry_is_unbaked(vpkpp_entry_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::entry(handle)->unbaked;
}

SOURCEPP_API void vpkpp_entry_free(vpkpp_entry_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::entry(*handle);
	*handle = nullptr;
}
