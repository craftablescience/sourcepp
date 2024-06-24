#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include <sourcepp/math/Integer.h>

namespace sourcepp::crypto {

std::array<std::byte, 16> computeMD5(const std::vector<std::byte>& buffer);

std::array<std::byte, 16> computeMD5(const std::byte* buffer, std::size_t len);

} // namespace sourcepp::crypto
