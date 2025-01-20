#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include <sourcepp/parser/Binary.h>
#include <sourcepp/Math.h>

namespace sourcepp::compression {

constexpr auto VALVE_LZMA_SIGNATURE = parser::binary::makeFourCC("LZMA");

[[nodiscard]] std::optional<std::vector<std::byte>> compressValveLZMA(std::span<const std::byte> data, uint8_t compressLevel = 6);

[[nodiscard]] std::optional<std::vector<std::byte>> decompressValveLZMA(std::span<const std::byte> data);

} // namespace sourcepp::compression
