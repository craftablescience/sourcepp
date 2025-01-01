#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

std::pair<std::string, std::string> computeSHA256KeyPair(uint16_t size = 2048);

bool verifySHA256PublicKey(std::span<const std::byte> buffer, std::span<const std::byte> publicKey, std::span<const std::byte> signature);

std::vector<std::byte> signDataWithSHA256PrivateKey(std::span<const std::byte> buffer, std::span<const std::byte> privateKey);

} // namespace sourcepp::crypto
