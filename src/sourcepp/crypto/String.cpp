#include <sourcepp/crypto/String.h>

#include <cryptopp/hex.h>

using namespace sourcepp;

std::vector<std::byte> crypto::decodeHexString(std::string_view hex) {
	std::string hexBin;
	CryptoPP::StringSource hexSource{hex.data(), true, new CryptoPP::HexDecoder{new CryptoPP::StringSink{hexBin}}};

	std::vector<std::byte> out;
	for (char c : hexBin) {
		out.push_back(static_cast<std::byte>(c));
	}
	return out;
}
