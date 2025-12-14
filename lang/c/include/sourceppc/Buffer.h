#pragma once

#include "API.h"

SOURCEPP_EXTERN typedef struct {
	int64_t size;
	void* data;
} sourcepp_buffer_t;

SOURCEPP_API sourcepp_buffer_t sourcepp_buffer_new(size_t size);

SOURCEPP_API void sourcepp_buffer_free(sourcepp_buffer_t* buffer);

SOURCEPP_API sourcepp_buffer_t sourcepp_buffer_invalid();

// These really only exist as a bit of a hack to tell the user the data type they contain
typedef sourcepp_buffer_t sourcepp_buffer_uint32_t;
typedef sourcepp_buffer_t sourcepp_buffer_uint64_t;
