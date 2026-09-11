#include <sourcepp/crypto/SHA1.h>

#include <BufferStream.h>
#include <cryptopp/sha.h>

using namespace sourcepp;

std::array<std::byte, 20> crypto::computeSHA1(std::span<const std::byte> buffer) {
	if (buffer.empty()) {
		return {};
	}

	CryptoPP::SHA1 sha1;
	sha1.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());

	std::array<std::byte, 20> final{};
	sha1.Final(reinterpret_cast<CryptoPP::byte*>(final.data()));
	return final;
}

uint32_t crypto::computeSHA1Partial(std::span<const std::byte> buffer) {
	if (buffer.empty()) {
		return {};
	}

	class SHA1P : public CryptoPP::SHA1 { public: using SHA1::StateBuf; };
	SHA1P sha1p;
	sha1p.Update(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());

	return sha1p.StateBuf()[0];
}
