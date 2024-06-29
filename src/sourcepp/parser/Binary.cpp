#include <sourcepp/parser/Binary.h>

#include <BufferStream.h>

using namespace sourcepp;

void parser::binary::readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom, std::size_t subtractFromOffset) {
	int offset = stream.read<int32_t>();
	if (offset == 0) {
		str = "";
		return;
	}

	auto pos = stream.tell();
	stream.seek(offset - static_cast<int64_t>(subtractFromOffset), offsetFrom);
	stream.read(str);
	stream.seek_u(pos);
}
