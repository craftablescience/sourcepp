#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace sourcepp::crypto {

std::vector<std::byte> decodeHexString(std::string_view hex);

std::string encodeHexString(std::span<const std::byte> hex);

} // namespace sourcepp::crypto
