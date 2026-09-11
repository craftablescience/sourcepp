#include <sourcepp/crypto/SHA256.h>

#include <cryptopp/sha.h>

using namespace sourcepp;

std::array<std::byte, 32> crypto::computeSHA256(std::span<const std::byte> buffer) {
	if (buffer.empty()) {
		return {};
	}

	CryptoPP::SHA256 sha256;
	sha256.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());

	std::array<std::byte, 32> final{};
	sha256.Final(reinterpret_cast<CryptoPP::byte*>(final.data()));
	return final;
}
