#pragma once

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

#include <string>
#include <string_view>
#include <vector>

#include "Buffer.h"
#include "String.h"

namespace Convert {

sourcepp_string_t toString(std::string_view str);

sourcepp_buffer_t toBuffer(const std::vector<std::byte>& vec);

sourcepp_string_array_t toStringArray(const std::vector<std::string>& stringVec);

size_t writeStringToMem(std::string_view str, char* buffer, size_t bufferLen);

size_t writeVectorToMem(const std::vector<std::byte>& vec, unsigned char* buffer, size_t bufferLen);

} // namespace Convert
