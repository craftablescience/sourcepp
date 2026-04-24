#include <sourcepp/crypto/SHA256.h>

#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>

using namespace sourcepp;

std::array<std::byte, 32> crypto::computeSHA256(std::span<const std::byte> buffer) {
	if (!LTM_MATH || SHA256_INDEX < 0 || buffer.empty()) {
		return {};
	}

	hash_state sha256;
	sha256_init(&sha256);
	sha256_process(&sha256, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());

	std::array<std::byte, 32> final{};
	sha256_done(&sha256, reinterpret_cast<unsigned char*>(final.data()));
	return final;
}
