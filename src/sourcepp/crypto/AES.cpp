#include <sourcepp/crypto/AES.h>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

using namespace sourcepp;

namespace sourcepp::crypto {

const std::array<std::byte, 16> NULL_IV{};

} // namespace sourcepp::crypto

bool crypto::decryptAES_CFB(std::span<std::byte> buffer, std::span<const std::byte> key, std::span<const std::byte> iv) {
	if (buffer.empty()) {
		return false;
	}

	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfb;
	cfb.SetKeyWithIV(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size(), reinterpret_cast<const CryptoPP::byte*>(iv.data()), iv.size());
	cfb.ProcessData(reinterpret_cast<CryptoPP::byte*>(buffer.data()), reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());
	return true;
}

bool crypto::encryptAES_CFB(std::span<std::byte> buffer, std::span<const std::byte> key, std::span<const std::byte> iv) {
	if (buffer.empty()) {
		return false;
	}

	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfb;
	cfb.SetKeyWithIV(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size(), reinterpret_cast<const CryptoPP::byte*>(iv.data()), iv.size());
	cfb.ProcessData(reinterpret_cast<CryptoPP::byte*>(buffer.data()), reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size());
	return true;
}
