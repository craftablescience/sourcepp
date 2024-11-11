#include <sourcepp/parser/Binary.h>

using namespace sourcepp;

void parser::binary::readStringAtOffset(BufferStream& stream, std::string& str, BufferStream::BufferStreamSeekDir offsetFrom, std::size_t subtractFromOffset) {
	const auto offset = stream.read<int32_t>();
	if (offset == 0) {
		str = "";
		return;
	}

	const auto pos = stream.tell();
	stream.seek(offset - static_cast<int64_t>(subtractFromOffset), offsetFrom);
	stream.read(str);
	stream.seek_u(pos);
}
