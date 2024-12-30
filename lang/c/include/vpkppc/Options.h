#pragma once

#include <sourceppc/API.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	ENTRY_COMPRESSION_TYPE_NO_OVERRIDE = -1,
	ENTRY_COMPRESSION_TYPE_NO_COMPRESS =  0,
	ENTRY_COMPRESSION_TYPE_DEFLATE     =  8,
	ENTRY_COMPRESSION_TYPE_BZIP2       = 12,
	ENTRY_COMPRESSION_TYPE_LZMA        = 14,
	ENTRY_COMPRESSION_TYPE_ZSTD        = 93,
	ENTRY_COMPRESSION_TYPE_XZ          = 95,
} vpkpp_entry_compression_type_e;

typedef struct {
	int16_t zip_compressionTypeOverride;
	int16_t zip_compressionStrength;
	uint8_t gma_writeCRCs;
	uint8_t vpk_generateMD5Entries;
}  vpkpp_bake_options_t;

typedef struct {
	int16_t zip_compressionType;
	int16_t zip_compressionStrength;
	uint16_t vpk_preloadBytes;
	uint8_t vpk_saveToDirectory;
} vpkpp_entry_options_t;

#ifdef __cplusplus
} // extern "C"
#endif
