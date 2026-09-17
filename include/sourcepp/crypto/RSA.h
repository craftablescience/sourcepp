#pragma once

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

[[nodiscard]] std::pair<std::vector<std::byte>, std::vector<std::byte>> computeRSAKeyPair(uint16_t size = 4096);

[[nodiscard]] bool verifyPublicKeySHA256(std::span<const std::byte> buffer, std::span<const std::byte> publicKey, std::span<const std::byte> signature);

[[nodiscard]] std::vector<std::byte> signDataWithPrivateKeySHA256(std::span<const std::byte> buffer, std::span<const std::byte> privateKey, std::string_view passphrase = "");

} // namespace sourcepp::crypto
