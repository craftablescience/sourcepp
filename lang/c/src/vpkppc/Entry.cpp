#include <vpkppc/Entry.h>

#include <vpkpp/Entry.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vpkppc/Convert.hpp>

using namespace vpkpp;

SOURCEPP_API size_t vpkpp_entry_get_path(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::entry(handle)->path, buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_entry_get_parent_path(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::entry(handle)->getParentPath(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_entry_get_filename(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::entry(handle)->getFilename(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_entry_get_stem(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::entry(handle)->getStem(), buffer, bufferLen);
}

SOURCEPP_API size_t vpkpp_entry_get_extension(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::entry(handle)->getExtension(), buffer, bufferLen);
}

SOURCEPP_API void vpkpp_entry_free(vpkpp_entry_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::entry(*handle);
	*handle = nullptr;
}

SOURCEPP_API void vpkpp_entry_array_free(vpkpp_entry_handle_array_t* array) {
	SOURCEPP_EARLY_RETURN(array);

	if (array->data) {
		for (size_t i = 0; i < array->size; i++) {
			if (auto*& entry = array->data[i]) {
				vpkpp_entry_free(&entry);
			}
		}
		std::free(array->data);
		array->data = nullptr;
	}
	array->size = 0;
}

SOURCEPP_API size_t vpkpp_virtual_entry_get_name(vpkpp_virtual_entry_handle_t* handle, char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);
	SOURCEPP_EARLY_RETURN_VAL(buffer, 0);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, 0);

	return Convert::writeStringToMem(Convert::virtualEntry(handle)->name, buffer, bufferLen);
}

SOURCEPP_API bool vpkpp_virtual_entry_is_writable(vpkpp_virtual_entry_handle_t* handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::virtualEntry(handle)->writable;
}

SOURCEPP_API void vpkpp_virtual_entry_free(vpkpp_virtual_entry_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::entry(*handle);
	*handle = nullptr;
}

SOURCEPP_API void vpkpp_virtual_entry_array_free(vpkpp_virtual_entry_handle_array_t* array) {
	SOURCEPP_EARLY_RETURN(array);

	if (array->data) {
		for (size_t i = 0; i < array->size; i++) {
			if (auto*& entry = array->data[i]) {
				vpkpp_virtual_entry_free(&entry);
			}
		}
		std::free(array->data);
		array->data = nullptr;
	}
	array->size = 0;
}
