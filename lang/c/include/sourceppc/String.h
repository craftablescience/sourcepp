#pragma once

#include "API.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int64_t size;
	uint8_t* data;
} sourcepp_string_t;

#define SOURCEPP_STRING_INVALID (sourcepp_string_t{.size = -1, .data = NULL})

typedef struct {
	int64_t size;
	char** data;
} sourcepp_string_array_t;

#define SOURCEPP_STRING_ARRAY_INVALID (sourcepp_string_array_t{.size = -1, .data = NULL})

#ifdef __cplusplus
} // extern "C"
#endif

SOURCEPP_API sourcepp_string_t sourcepp_string_new(size_t size);

SOURCEPP_API void sourcepp_string_free(sourcepp_string_t* str);

SOURCEPP_API sourcepp_string_array_t sourcepp_string_array_new(size_t size);

SOURCEPP_API void sourcepp_string_array_free(sourcepp_string_array_t* array);
