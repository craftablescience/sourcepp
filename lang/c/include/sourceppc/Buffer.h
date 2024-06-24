#pragma once

#include "API.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int64_t size;
	uint8_t* data;
} sourcepp_buffer_t;

#define SOURCEPP_BUFFER_INVALID (sourcepp_buffer_t{.size = -1, .data = NULL})

#ifdef __cplusplus
} // extern "C"
#endif

SOURCEPP_API sourcepp_buffer_t sourcepp_buffer_new(size_t size);

SOURCEPP_API void sourcepp_buffer_free(sourcepp_buffer_t* buffer);
