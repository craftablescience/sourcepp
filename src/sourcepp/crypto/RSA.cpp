#include <sourcepp/crypto/RSA.h>

#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>

#include <sourcepp/String.h>

using namespace sourcepp;

std::pair<std::string, std::string> crypto::computeSHA256KeyPair(uint16_t size) {
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::RSAES_OAEP_SHA256_Decryptor privateKey{rng, size};
	CryptoPP::RSAES_OAEP_SHA256_Encryptor publicKey{privateKey};

	std::vector<CryptoPP::byte> privateKeyData;
	CryptoPP::VectorSink privateKeyDataSink{privateKeyData};
	privateKey.AccessMaterial().Save(privateKeyDataSink);

	std::vector<CryptoPP::byte> publicKeyData;
	CryptoPP::VectorSink publicKeyDataArraySink{publicKeyData};
	publicKey.AccessMaterial().Save(publicKeyDataArraySink);

	return {
		string::encodeHex({reinterpret_cast<const std::byte*>(privateKeyData.data()), privateKeyData.size()}),
		string::encodeHex({reinterpret_cast<const std::byte*>(publicKeyData.data()), publicKeyData.size()}),
	};
}

bool crypto::verifySHA256PublicKey(std::span<const std::byte> buffer, std::span<const std::byte> publicKey, std::span<const std::byte> signature) {
	CryptoPP::ArraySource publicKeySource{reinterpret_cast<const CryptoPP::byte*>(publicKey.data()), publicKey.size(), true};
	const CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Verifier verifier{publicKeySource};
	return verifier.VerifyMessage(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size(), reinterpret_cast<const CryptoPP::byte*>(signature.data()), signature.size());
}

std::vector<std::byte> crypto::signDataWithSHA256PrivateKey(std::span<const std::byte> buffer, std::span<const std::byte> privateKey) {
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::ArraySource privateKeySource{reinterpret_cast<const CryptoPP::byte*>(privateKey.data()), privateKey.size(), true};
	const CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Signer signer{privateKeySource};

	std::vector<std::byte> out;
	CryptoPP::ArraySource signData{reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size(), true, new CryptoPP::SignerFilter{rng, signer, new CryptoPP::VectorSink{reinterpret_cast<std::vector<CryptoPP::byte>&>(out)}}};
	return out;
}
