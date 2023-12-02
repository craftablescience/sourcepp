#include <studiomodelpp/internal/Helpers.h>

#include <studiomodelpp/internal/BufferStream.h>

using namespace studiomodelpp;

void internal::readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom) {
	int offset = stream.read<int>();
	auto pos = stream.tell();
	stream.seek(offset, offsetFrom);
	stream.read(str);
	stream.seek(pos);
}
