#include <sourcepp/crypto/SHA1.h>

#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>

using namespace sourcepp;

std::array<std::byte, 20> crypto::computeSHA1(std::span<const std::byte> buffer) {
	if (!LTM_MATH || buffer.empty()) {
		return {};
	}

	hash_state sha1;
	sha1_init(&sha1);
	sha1_process(&sha1, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());

	std::array<std::byte, 20> final{};
	sha1_done(&sha1, reinterpret_cast<unsigned char*>(final.data()));
	return final;
}
