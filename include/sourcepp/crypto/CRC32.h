#pragma once

#include <cstddef>
#include <span>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

uint32_t computeCRC32(std::span<const std::byte> buffer);

} // namespace sourcepp::crypto
