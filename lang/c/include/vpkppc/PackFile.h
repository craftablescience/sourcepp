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

typedef void(*EntryCallback)(const char* path, vpkpp_entry_handle_t entry);

typedef int(*EntryPredicate)(const char* path, vpkpp_entry_handle_t entry);

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open(const char* path);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_callback(const char* path, EntryCallback callback);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_options(const char* path, vpkpp_pack_file_options_t options);

// REQUIRES MANUAL FREE: vpkpp_close
SOURCEPP_API vpkpp_pack_file_handle_t vpkpp_open_with_options_and_callback(const char* path, vpkpp_pack_file_options_t options, EntryCallback callback);

SOURCEPP_API vpkpp_pack_file_type_e vpkpp_get_type(vpkpp_pack_file_handle_t handle);

SOURCEPP_API vpkpp_pack_file_options_t vpkpp_get_options(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_has_entry_checksums(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_array_free
SOURCEPP_API sourcepp_string_array_t vpkpp_verify_entry_checksums(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_has_pack_file_checksum(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_verify_pack_file_checksum(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_has_pack_file_signature(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_verify_pack_file_signature(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_is_case_sensitive(vpkpp_pack_file_handle_t handle);

SOURCEPP_API int vpkpp_has_entry(vpkpp_pack_file_handle_t handle, const char* path, int includeUnbaked);

// REQUIRES MANUAL FREE: vpkpp_entry_free
SOURCEPP_API vpkpp_entry_handle_t vpkpp_find_entry(vpkpp_pack_file_handle_t handle, const char* path, int includeUnbaked);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vpkpp_read_entry(vpkpp_pack_file_handle_t handle, const char* path);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_read_entry_text(vpkpp_pack_file_handle_t handle, const char* path);

SOURCEPP_API int vpkpp_is_read_only(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_add_entry_from_file(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath);

SOURCEPP_API void vpkpp_add_entry_from_file_with_options(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath, vpkpp_entry_options_t options);

SOURCEPP_API void vpkpp_add_entry_from_mem(vpkpp_pack_file_handle_t handle, const char* path, const unsigned char* buffer, size_t bufferLen);

SOURCEPP_API void vpkpp_add_entry_from_mem_with_options(vpkpp_pack_file_handle_t handle, const char* path, const unsigned char* buffer, size_t bufferLen, vpkpp_entry_options_t options);

SOURCEPP_API int vpkpp_rename_entry(vpkpp_pack_file_handle_t handle, const char* oldPath, const char* newPath);

SOURCEPP_API int vpkpp_rename_directory(vpkpp_pack_file_handle_t handle, const char* oldDir, const char* newDir);

SOURCEPP_API int vpkpp_remove_entry(vpkpp_pack_file_handle_t handle, const char* path);

SOURCEPP_API int vpkpp_remove_directory(vpkpp_pack_file_handle_t handle, const char* dirName);

SOURCEPP_API int vpkpp_bake(vpkpp_pack_file_handle_t handle, const char* outputDir);

SOURCEPP_API int vpkpp_bake_with_callback(vpkpp_pack_file_handle_t handle, const char* outputDir, EntryCallback callback);

SOURCEPP_API int vpkpp_extract_entry(vpkpp_pack_file_handle_t handle, const char* entryPath, const char* filepath);

SOURCEPP_API int vpkpp_extract_directory(vpkpp_pack_file_handle_t handle, const char* dir, const char* outputDir);

SOURCEPP_API int vpkpp_extract_all(vpkpp_pack_file_handle_t handle, const char* outputDir, int createUnderPackFileDir);

SOURCEPP_API int vpkpp_extract_all_if(vpkpp_pack_file_handle_t handle, const char* outputDir, EntryPredicate predicate, int stripSharedDirs);

SOURCEPP_API size_t vpkpp_get_entry_count(vpkpp_pack_file_handle_t handle, int includeUnbaked);

SOURCEPP_API void vpkpp_run_for_all_entries(vpkpp_pack_file_handle_t handle, EntryCallback operation, int includeUnbaked);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_get_filepath(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filepath(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_get_filename(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filename(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_get_filestem(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_get_truncated_filestem(vpkpp_pack_file_handle_t handle);

SOURCEPP_API vpkpp_attribute_e vpkpp_get_supported_entry_attributes(vpkpp_pack_file_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_to_string(vpkpp_pack_file_handle_t handle);

SOURCEPP_API void vpkpp_close(vpkpp_pack_file_handle_t* handle);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vpkpp_escape_entry_path_for_write(const char* path);

// REQUIRES MANUAL FREE: sourcepp_string_array_free
SOURCEPP_API sourcepp_string_array_t vpkpp_get_supported_file_types();
