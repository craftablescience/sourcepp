#pragma once

#include <array>
#include <cstddef>
#include <span>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

std::array<std::byte, 20> computeSHA1(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
