#pragma once

#include "API.h"

SOURCEPP_EXTERN typedef struct {
	int64_t size;
	char* data;
} sourcepp_string_t;

#define SOURCEPP_STRING_INVALID (SOURCEPP_CAST_CTOR(sourcepp_string_t) {.size = -1, .data = NULL})

SOURCEPP_API sourcepp_string_t sourcepp_string_new(size_t size);

SOURCEPP_API void sourcepp_string_free(sourcepp_string_t* str);

SOURCEPP_EXTERN typedef struct {
	int64_t size;
	char** data;
} sourcepp_string_array_t;

#define SOURCEPP_STRING_ARRAY_INVALID (SOURCEPP_CAST_CTOR(sourcepp_string_array_t) {.size = -1, .data = NULL})

SOURCEPP_API sourcepp_string_array_t sourcepp_string_array_new(size_t size);

SOURCEPP_API void sourcepp_string_array_free(sourcepp_string_array_t* array);
