#include <sourcepp/buffer/Buffer.h>

#include <BufferStream.h>

#include <sourcepp/math/Integer.h>

using namespace sourcepp;

void buffer::readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom, std::size_t subtractFromOffset) {
	int offset = stream.read<int32_t>();
	if (offset == 0) {
		str = "";
		return;
	}

	auto pos = stream.tell();
	stream.seek(offset - subtractFromOffset, offsetFrom);
	stream.read(str);
	stream.seek(pos);
}