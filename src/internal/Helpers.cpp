#include <studiomodelpp/internal/Helpers.h>

#include <studiomodelpp/internal/BufferStream.h>

using namespace studiomodelpp;

void internal::readStringAtOffset(BufferStream& stream, std::string& str) {
	int offset = stream.read<int>();
	auto pos = stream.tell();
	stream.seek(offset, std::ios::cur);
	stream.read(str);
	stream.seek(pos);
}
