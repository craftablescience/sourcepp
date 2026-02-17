#pragma once

#include "API.h"

SOURCEPP_EXTERN typedef struct {
	int64_t size;
	uint8_t* data;
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

// C++ conversion routines
#ifdef __cplusplus

#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace sourceppc::convert {

sourcepp_string_t toString(std::string_view str);

sourcepp_string_array_t toStringArray(const std::vector<std::string>& stringVec);

size_t writeStringToMem(std::string_view str, char* buffer, size_t bufferLen);

size_t writeVectorToMem(const std::vector<std::byte>& vec, unsigned char* buffer, size_t bufferLen);

// requires clause copied from BufferStream - not including here because that header is HEAVY
template<typename T>
requires std::is_trivial_v<T> && std::is_standard_layout_v<T> && (!std::is_pointer_v<T>)
size_t writeVectorToMem(const std::vector<T>& vec, T* buffer, size_t bufferLen) {
	if (vec.size() >= bufferLen) {
		std::memcpy(buffer, vec.data(), sizeof(T) * bufferLen);
		return bufferLen;
	}
	std::memcpy(buffer, vec.data(), sizeof(T) * vec.size());
	return vec.size();
}

} // namespace sourceppc::convert

#endif
