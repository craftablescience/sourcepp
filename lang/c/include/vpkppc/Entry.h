#pragma once

#include <sourceppc/API.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* vpkpp_entry_handle_t;

typedef struct {
	int64_t size;
	vpkpp_entry_handle_t* data;
} vpkpp_entry_handle_array_t;

#define VPKPP_ENTRY_HANDLE_ARRAY_INVALID (vpkpp_entry_handle_array_t{.size = -1, .data = NULL})

#ifdef __cplusplus
} // extern "C"
#endif

SOURCEPP_API size_t vpkpp_entry_get_path(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_entry_get_parent_path(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_entry_get_filename(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_entry_get_stem(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API size_t vpkpp_entry_get_extension(vpkpp_entry_handle_t handle, char* buffer, size_t bufferLen);

SOURCEPP_API void vpkpp_entry_free(vpkpp_entry_handle_t* handle);

SOURCEPP_API void vpkpp_entry_array_free(vpkpp_entry_handle_array_t* array);
