#pragma once

#include "API.h"

SOURCEPP_EXTERN typedef struct {
	uint8_t* data;
	int64_t size;
} sourcepp_string_t;

#define SOURCEPP_STRING_INVALID (SOURCEPP_CAST_CTOR(sourcepp_string_t) {.data = NULL, .size = -1})

SOURCEPP_API sourcepp_string_t sourcepp_string_new(size_t size);
SOURCEPP_API void sourcepp_string_free(sourcepp_string_t* str);

SOURCEPP_EXTERN typedef struct {
	char** data;
	int64_t size;
} sourcepp_string_array_t;

#define SOURCEPP_STRING_ARRAY_INVALID (SOURCEPP_CAST_CTOR(sourcepp_string_array_t) {.data = NULL, .size = -1})

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

SOURCEPP_EXPORT sourcepp_string_t toString(std::string_view str);

SOURCEPP_EXPORT sourcepp_string_array_t toStringArray(const std::vector<std::string>& stringVec);

SOURCEPP_EXPORT size_t writeStringToMem(std::string_view str, char* buffer, size_t bufferLen);

SOURCEPP_EXPORT size_t writeVectorToMem(const std::vector<std::byte>& vec, unsigned char* buffer, size_t bufferLen);

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
