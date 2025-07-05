#pragma once

#include "API.h"

SOURCEPP_STRUCT_BEGIN() {
	SOURCEPP_STRUCT_FIELD(int64_t,  size, -1);
	SOURCEPP_STRUCT_FIELD(uint8_t*, data, NULL);
} SOURCEPP_STRUCT_END(sourcepp, string);

#define SOURCEPP_STRING_INVALID (SOURCEPP_CAST_CTOR(sourcepp_string_t) {.size = -1, .data = NULL})

SOURCEPP_STRUCT_BEGIN() {
	SOURCEPP_STRUCT_FIELD(int64_t, size, -1);
	SOURCEPP_STRUCT_FIELD(char**,  data, NULL);
} SOURCEPP_STRUCT_END(sourcepp, string_array);

#define SOURCEPP_STRING_ARRAY_INVALID (SOURCEPP_CAST_CTOR(sourcepp_string_array_t) {.size = -1, .data = NULL})

SOURCEPP_STATIC(sourcepp, string, sourcepp_string_t, new, size_t size);
SOURCEPP_STATIC(sourcepp, string,              void, free, sourcepp_string_t* str);

SOURCEPP_STATIC(sourcepp, string_array, sourcepp_string_array_t, new, size_t size);
SOURCEPP_STATIC(sourcepp, string_array,                    void, free, sourcepp_string_array_t* array);
