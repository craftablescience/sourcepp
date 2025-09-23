#include <vcryptpp/VICE.h>

#include <cstring>

#include <IceKey.h>

using namespace vcryptpp;

namespace {

std::vector<std::byte> applyIce(std::span<const std::byte> data, std::string_view code, bool encrypt) {
	if (code.size() != IceKey::blockSize()) {
		return {};
	}

	IceKey ice;
	ice.set(reinterpret_cast<const unsigned char*>(code.data()));

	const auto* in = reinterpret_cast<const unsigned char*>(data.data());

	std::vector<std::byte> outVec(data.size());
	auto* out = reinterpret_cast<unsigned char*>(outVec.data());

	auto bytesLeft = data.size();
	while (bytesLeft >= IceKey::blockSize()) {
		if (encrypt) {
			ice.encrypt(in, out);
		} else {
			ice.decrypt(in, out);
		}
		bytesLeft -= IceKey::blockSize();
		in += IceKey::blockSize();
		out += IceKey::blockSize();
	}
	if (bytesLeft) {
		std::memcpy(out, in, bytesLeft);
	}

	return outVec;
}

} // namespace

std::vector<std::byte> VICE::encrypt(std::span<const std::byte> data, std::string_view code) {
	return ::applyIce(data, code, true);
}

std::vector<std::byte> VICE::decrypt(std::span<const std::byte> data, std::string_view code) {
	return ::applyIce(data, code, false);
}
