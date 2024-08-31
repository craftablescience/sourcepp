#pragma once

#include <sourceppc/API.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int32_t gma_writeCRCs;
	int32_t vpk_generateMD5Entries;
} vpkpp_bake_options_t;

typedef struct {
	int32_t vpk_saveToDirectory;
	uint32_t vpk_preloadBytes;
} vpkpp_entry_options_t;

#ifdef __cplusplus
} // extern "C"
#endif
