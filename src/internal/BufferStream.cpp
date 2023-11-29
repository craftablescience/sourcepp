#include <studiomodelpp/internal/BufferStream.h>

using namespace studiomodelpp::internal;

BufferStream::BufferStream(const std::byte* buffer, std::uint64_t bufferLength) {
    this->streamBuffer = buffer;
    this->streamLen = bufferLength;
    this->streamPos = 0;
}

void BufferStream::seek(std::uint64_t offset, std::ios::seekdir offsetFrom) {
    if (offsetFrom == std::ios::beg)
        this->streamPos = offset;
    else if (offsetFrom == std::ios::cur)
        this->streamPos += offsetFrom;
    else if (offsetFrom == std::ios::end)
        this->streamPos = this->streamLen + offsetFrom;
}

void BufferStream::skip(std::uint64_t offset) {
	this->seek(offset, std::ios::cur);
}

std::uint64_t BufferStream::tell() const {
    return this->streamPos;
}

std::vector<std::byte> BufferStream::readBytes(std::uint64_t length) {
    std::vector<std::byte> out;
    out.resize(length);
    for (int i = 0; i < length; i++, this->streamPos++) {
		out.push_back(this->streamBuffer[this->streamPos]);
	}
    return out;
}

std::byte BufferStream::peek(long offset) {
    return this->streamBuffer[this->streamPos + offset];
}
