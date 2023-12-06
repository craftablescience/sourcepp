#include <studiomodelpp/internal/Helpers.h>

#include <BufferStream.h>

using namespace studiomodelpp;

void internal::readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom, std::size_t subtractFromOffset) {
	int offset = stream.read<int>();
	auto pos = stream.tell();
	stream.seek(offset - subtractFromOffset, offsetFrom);
	stream.read(str);
	stream.seek(pos);
}
