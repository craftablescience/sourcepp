#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

[[nodiscard]] std::pair<std::string, std::string> computeSHA256KeyPair(uint16_t size = 2048);

[[nodiscard]] bool verifySHA256PublicKey(std::span<const std::byte> buffer, std::span<const std::byte> publicKey, std::span<const std::byte> signature);

[[nodiscard]] std::vector<std::byte> signDataWithSHA256PrivateKey(std::span<const std::byte> buffer, std::span<const std::byte> privateKey);

} // namespace sourcepp::crypto
