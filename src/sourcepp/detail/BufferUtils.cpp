#include <sourcepp/detail/BufferUtils.h>

#include <BufferStream.h>

void sourcepp::detail::readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom, std::size_t subtractFromOffset) {
	int offset = stream.read<int>();
	if (offset == 0) {
		str = "";
		return;
	}

	auto pos = stream.tell();
	stream.seek(offset - subtractFromOffset, offsetFrom);
	stream.read(str);
	stream.seek(pos);
}
