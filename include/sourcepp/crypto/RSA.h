#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::crypto {

std::pair<std::string, std::string> computeSHA256KeyPair(uint16_t size = 2048);

bool verifySHA256PublicKey(std::span<const std::byte> buffer, const std::vector<std::byte>& publicKey, std::span<const std::byte> signature);

std::vector<std::byte> signDataWithSHA256PrivateKey(const std::vector<std::byte>& buffer, const std::vector<std::byte>& privateKey);

} // namespace sourcepp::crypto
