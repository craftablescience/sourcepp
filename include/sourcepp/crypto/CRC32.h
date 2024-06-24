#pragma once

#include <cstddef>
#include <vector>

#include <sourcepp/math/Integer.h>

namespace sourcepp::crypto {

uint32_t computeCRC32(const std::vector<std::byte>& buffer);

uint32_t computeCRC32(const std::byte* buffer, std::size_t len);

} // namespace sourcepp::crypto
