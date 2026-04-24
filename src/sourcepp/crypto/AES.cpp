#include <sourcepp/crypto/AES.h>

#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>

using namespace sourcepp;

namespace sourcepp::crypto {

const std::array<std::byte, 16> NULL_IV{};

} // namespace sourcepp::crypto

bool crypto::decryptAES_CFB(std::span<std::byte> buffer, std::span<const std::byte> key, std::span<const std::byte> iv) {
	if (!LTM_MATH || AES_INDEX < 0 || buffer.empty()) {
		return {};
	}

	const std::unique_ptr<symmetric_CFB, void(*)(symmetric_CFB*)> cfb{new symmetric_CFB, [](symmetric_CFB* c) { cfb_done(c); delete c; }};
	if (cfb_start(AES_INDEX, reinterpret_cast<const unsigned char*>(iv.data()), reinterpret_cast<const unsigned char*>(key.data()), static_cast<int>(key.size()), 0, cfb.get()) != CRYPT_OK) {
		return {};
	}
	const unsigned long bufferLen = buffer.size();
	return cfb_decrypt(reinterpret_cast<const unsigned char*>(buffer.data()), reinterpret_cast<unsigned char*>(buffer.data()), bufferLen, cfb.get()) == CRYPT_OK;
}

bool crypto::encryptAES_CFB(std::span<std::byte> buffer, std::span<const std::byte> key, std::span<const std::byte> iv) {
	if (!LTM_MATH || AES_INDEX < 0 || buffer.empty()) {
		return {};
	}

	const std::unique_ptr<symmetric_CFB, void(*)(symmetric_CFB*)> cfb{new symmetric_CFB, [](symmetric_CFB* c) { cfb_done(c); delete c; }};
	if (cfb_start(AES_INDEX, reinterpret_cast<const unsigned char*>(iv.data()), reinterpret_cast<const unsigned char*>(key.data()), static_cast<int>(key.size()), 0, cfb.get()) != CRYPT_OK) {
		return {};
	}
	const unsigned long bufferLen = buffer.size();
	return cfb_encrypt(reinterpret_cast<const unsigned char*>(buffer.data()), reinterpret_cast<unsigned char*>(buffer.data()), bufferLen, cfb.get()) == CRYPT_OK;
}
