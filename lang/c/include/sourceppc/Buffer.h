#pragma once

#include "API.h"

SOURCEPP_EXTERN typedef struct {
	int64_t size;
	void* data;
} sourcepp_buffer_t;

#define SOURCEPP_BUFFER_INVALID (SOURCEPP_CAST_CTOR(sourcepp_buffer_t) {.size = -1, .data = NULL})

SOURCEPP_API sourcepp_buffer_t sourcepp_buffer_new(size_t size);

SOURCEPP_API void sourcepp_buffer_free(sourcepp_buffer_t* buffer);

typedef sourcepp_buffer_t sourcepp_buffer_uint8_t;
typedef sourcepp_buffer_t sourcepp_buffer_uint16_t;
typedef sourcepp_buffer_t sourcepp_buffer_uint32_t;
typedef sourcepp_buffer_t sourcepp_buffer_uint64_t;
