#pragma once

#include <array>
#include <cstddef>
#include <span>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

std::array<std::byte, 16> computeMD5(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
