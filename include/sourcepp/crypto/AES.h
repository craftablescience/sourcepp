#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

extern const std::array<std::byte, 16> NULL_IV;

bool decryptAES_CFB(std::span<std::byte> buffer, std::span<const std::byte> key, std::span<const std::byte> iv = NULL_IV);

bool encryptAES_CFB(std::span<std::byte> buffer, std::span<const std::byte> key, std::span<const std::byte> iv = NULL_IV);

} // namespace sourcepp::crypto
