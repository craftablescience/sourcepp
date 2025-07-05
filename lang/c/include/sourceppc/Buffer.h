#pragma once

#include "API.h"

SOURCEPP_STRUCT_BEGIN() {
	SOURCEPP_STRUCT_FIELD(int64_t, size, -1);
	SOURCEPP_STRUCT_FIELD(void*,   data, NULL);
} SOURCEPP_STRUCT_END(sourcepp, buffer);

#define SOURCEPP_BUFFER_INVALID (SOURCEPP_CAST_CTOR(sourcepp_buffer_t) {.size = -1, .data = NULL})

SOURCEPP_STATIC(sourcepp, buffer, sourcepp_buffer_t, new, size_t size);
SOURCEPP_STATIC(sourcepp, buffer,              void, free, sourcepp_buffer_t* buffer);

// These really only exist as a bit of a hack to tell the user the data type they contain
SOURCEPP_TYPEDF(sourcepp, buffer_uint32, sourcepp_buffer_t);
SOURCEPP_TYPEDF(sourcepp, buffer_uint64, sourcepp_buffer_t);
