#pragma once

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

#include <sourcepp/math/Integer.h>

namespace vcryptpp::VFONT {

constexpr std::string_view IDENTIFIER = "VFONT1";

constexpr uint8_t MAGIC = 167;

[[nodiscard]] std::vector<std::byte> decrypt(std::span<const std::byte> data);

} // namespace vcryptpp::VFONT
