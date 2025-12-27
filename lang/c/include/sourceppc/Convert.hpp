#pragma once

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

#include <cstring>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "Buffer.h"
#include "String.h"

namespace sourceppc::c {

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

sourcepp_string_t toString(std::string_view str);

sourcepp_string_t toString(const std::filesystem::path& path);

sourcepp_string_array_t toStringArray(std::span<const std::string_view> stringVec);

sourcepp_string_array_t toStringArray(std::span<const std::filesystem::path> stringVec);

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

} // namespace sourceppc::c
