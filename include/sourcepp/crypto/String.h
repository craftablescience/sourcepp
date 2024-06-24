#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

namespace sourcepp::crypto {

std::vector<std::byte> decodeHexString(std::string_view hex);

} // namespace sourcepp::crypto
