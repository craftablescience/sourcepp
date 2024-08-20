#pragma once

#include <sourceppc/API.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int gma_writeCRCs;
	uint32_t vpk_version;
	uint32_t vpk_preferredChunkSize;
	int vpk_generateMD5Entries;
	uint16_t zip_compressionMethod;
} vpkpp_pack_file_options_t;

typedef struct {
	int vpk_saveToDirectory;
	uint32_t vpk_preloadBytes;
} vpkpp_entry_options_t;

#ifdef __cplusplus
} // extern "C"
#endif
