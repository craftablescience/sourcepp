#include <sourcepp/crypto/String.h>

#include <iomanip>
#include <sstream>

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

std::string crypto::encodeHexString(std::span<const std::byte> hex) {
	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (auto byte : hex) {
		oss << std::setw(2) << static_cast<int>(std::to_integer<unsigned char>(byte));
	}
	return oss.str();
}
