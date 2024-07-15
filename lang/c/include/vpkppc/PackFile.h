#pragma once

#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#include "Attribute.h"
#include "Entry.h"
#include "Options.h"
#include "PackFileType.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* vpkpp_pack_file_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_options(const char* path, vpkpp_pack_file_options_t options);

SOURCEPP_API vpkpp_pack_file_type_e vpkpp_get_type(vpkpp_pack_file_handle_t handle);

SOURCEPP_API vpkpp_pack_file_options_t vpkpp_get_options(vpkpp_pack_file_handle_t handle);

SOURCEPP_API bool vpkpp_has_entry_checksums(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_array_free
SOURCEPP_API sourcepp_string_array_t vpkpp_verify_entry_checksums(vpkpp_pack_file_handle_t handle);

SOURCEPP_API bool vpkpp_has_file_checksum(vpkpp_pack_file_handle_t handle);

SOURCEPP_API bool vpkpp_verify_file_checksum(vpkpp_pack_file_handle_t handle);

SOURCEPP_API bool vpkpp_has_file_signature(vpkpp_pack_file_handle_t handle);

SOURCEPP_API bool vpkpp_verify_file_signature(vpkpp_pack_file_handle_t handle);

SOURCEPP_API bool vpkpp_is_case_sensitive(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: vpkpp_entry_free
SOURCEPP_API vpkpp_entry_handle_t vpkpp_find_entry(vpkpp_pack_file_handle_t handle, const char* filename, bool includeUnbaked);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vpkpp_read_entry(vpkpp_pack_file_handle_t handle, vpkpp_entry_handle_t entry);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_read_entry_text(vpkpp_pack_file_handle_t handle, vpkpp_entry_handle_t entry);

SOURCEPP_API bool vpkpp_is_read_only(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_add_entry_from_file(vpkpp_pack_file_handle_t handle, const char* filename, const char* pathToFile);

SOURCEPP_API void vpkpp_add_entry_from_mem(vpkpp_pack_file_handle_t handle, const char* filename, const unsigned char* buffer, size_t bufferLen);

SOURCEPP_API bool vpkpp_remove_entry(vpkpp_pack_file_handle_t handle, const char* filename);

SOURCEPP_API bool vpkpp_bake(vpkpp_pack_file_handle_t handle, const char* outputDir);

SOURCEPP_API bool vpkpp_extract_entry(vpkpp_pack_file_handle_t handle, vpkpp_entry_handle_t entry, const char* filePath);

SOURCEPP_API bool vpkpp_extract_dir(vpkpp_pack_file_handle_t handle, const char* dir, const char* outputDir);

SOURCEPP_API bool vpkpp_extract_all(vpkpp_pack_file_handle_t handle, const char* outputDir, bool createUnderPackFileDir);

SOURCEPP_API bool vpkpp_extract_all_if(vpkpp_pack_file_handle_t handle, const char* outputDir, bool(*predicate)(vpkpp_entry_handle_t));

// REQUIRES MANUAL FREE: vpkpp_entry_array_free
SOURCEPP_API vpkpp_entry_handle_array_t vpkpp_get_baked_entries(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: vpkpp_entry_array_free
SOURCEPP_API vpkpp_entry_handle_array_t vpkpp_get_unbaked_entries(vpkpp_pack_file_handle_t handle);

SOURCEPP_API size_t vpkpp_get_entry_count(vpkpp_pack_file_handle_t handle, bool includeUnbaked);

SOURCEPP_API size_t vpkpp_get_filepath(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_get_truncated_filepath(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_get_filename(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_get_truncated_filename(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_get_filestem(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_get_truncated_filestem(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_get_supported_entry_attributes(vpkpp_pack_file_handle_t handle, vpkpp_attribute_e* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_to_string(vpkpp_pack_file_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API void vpkpp_close(vpkpp_pack_file_handle_t* handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_escape_entry_path(const char* path);

// REQUIRES MANUAL FREE: sourcepp_string_array_free
SOURCEPP_API sourcepp_string_array_t vpkpp_get_supported_file_types();
