#include <sourcepp/crypto/MD5.h>

#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>

using namespace sourcepp;

std::array<std::byte, 16> crypto::computeMD5(std::span<const std::byte> buffer) {
	if (!LTM_MATH || buffer.empty()) {
		return {};
	}

	hash_state md5;
	md5_init(&md5);
	md5_process(&md5, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());

	std::array<std::byte, 16> final{};
	md5_done(&md5, reinterpret_cast<unsigned char*>(final.data()));
	return final;
}
