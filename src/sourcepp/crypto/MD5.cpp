#include <sourcepp/crypto/MD5.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

using namespace sourcepp;

std::array<std::byte, 16> crypto::computeMD5(std::span<const std::byte> buffer) {
	// Make sure this is right
	static_assert(CryptoPP::Weak::MD5::DIGESTSIZE == sizeof(std::array<std::byte, 16>));

	CryptoPP::Weak::MD5 md5;
	md5.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());

	std::array<std::byte, 16> final{};
	md5.Final(reinterpret_cast<CryptoPP::byte*>(final.data()));
	return final;
}
