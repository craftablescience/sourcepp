#include <sourcepp/crypto/RSA.h>

#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>

using namespace sourcepp;

std::pair<std::string, std::string> crypto::computeSHA256KeyPair(uint16_t size) {
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::RSAES_OAEP_SHA256_Decryptor privateKey{rng, size};
	CryptoPP::RSAES_OAEP_SHA256_Encryptor publicKey{privateKey};

	std::vector<CryptoPP::byte> privateKeyData;
	CryptoPP::VectorSink privateKeyDataSink{privateKeyData};
	privateKey.AccessMaterial().Save(privateKeyDataSink);
	std::string privateKeyStr;
	CryptoPP::StringSource privateKeyStringSource{privateKeyData.data(), privateKeyData.size(), true, new CryptoPP::HexEncoder{new CryptoPP::StringSink{privateKeyStr}}};

	std::vector<CryptoPP::byte> publicKeyData;
	CryptoPP::VectorSink publicKeyDataArraySink{publicKeyData};
	publicKey.AccessMaterial().Save(publicKeyDataArraySink);
	std::string publicKeyStr;
	CryptoPP::StringSource publicKeyStringSource{publicKeyData.data(), publicKeyData.size(), true, new CryptoPP::HexEncoder{new CryptoPP::StringSink{publicKeyStr}}};

	return std::make_pair(std::move(privateKeyStr), std::move(publicKeyStr));
}

bool crypto::verifySHA256PublicKey(const std::vector<std::byte>& buffer, const std::vector<std::byte>& publicKey, const std::vector<std::byte>& signature) {
	CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Verifier verifier{
		CryptoPP::VectorSource(reinterpret_cast<const std::vector<CryptoPP::byte>&>(publicKey), true).Ref()
	};
	return verifier.VerifyMessage(reinterpret_cast<const CryptoPP::byte*>(buffer.data()), buffer.size(),
	                              reinterpret_cast<const CryptoPP::byte*>(signature.data()), signature.size());
}

std::vector<std::byte> crypto::signDataWithSHA256PrivateKey(const std::vector<std::byte>& buffer, const std::vector<std::byte>& privateKey) {
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Signer signer{
		CryptoPP::VectorSource(reinterpret_cast<const std::vector<CryptoPP::byte>&>(privateKey), true).Ref()
	};

	std::vector<std::byte> out;
	CryptoPP::VectorSource signData{
		reinterpret_cast<const std::vector<CryptoPP::byte> &>(buffer), true,
		new CryptoPP::SignerFilter{rng, signer, new CryptoPP::VectorSink{reinterpret_cast<std::vector<CryptoPP::byte> &>(out)}}
	};
	return out;
}
