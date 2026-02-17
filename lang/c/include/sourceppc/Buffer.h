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

// C++ conversion routines
#ifdef __cplusplus
#include <cstring>
#include <type_traits>
#include <vector>

namespace sourceppc::convert {

template<typename T>
requires (std::is_trivially_copyable_v<T> && !std::is_pointer_v<T>)
sourcepp_buffer_t toBuffer(const std::vector<T>& vec) {
	auto buf = sourcepp_buffer_new(vec.size() * sizeof(T));
	std::memcpy(buf.data, vec.data(), vec.size() * sizeof(T));
	return buf;
}

template<typename T>
requires (std::is_trivially_copyable_v<T> && !std::is_pointer_v<T>)
std::vector<T> fromBuffer(const sourcepp_buffer_t& buffer) {
	if (buffer.size % sizeof(T) != 0) {
		return {};
	}
	std::vector<T> vec(buffer.size / sizeof(T));
	std::memcpy(vec.data(), buffer.data, buffer.size / sizeof(T));
	return vec;
}

} // namespace sourceppc::convert

#endif
