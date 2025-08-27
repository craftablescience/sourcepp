#pragma once

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

#include <sourcepp/Math.h>

namespace vcryptpp::VFONT {

constexpr std::string_view SIGNATURE = "VFONT1";

constexpr uint8_t MAGIC = 167;

[[nodiscard]] std::vector<std::byte> encrypt(std::span<const std::byte> data, uint8_t saltLength = 2);

[[nodiscard]] std::vector<std::byte> decrypt(std::span<const std::byte> data);

} // namespace vcryptpp::VFONT
