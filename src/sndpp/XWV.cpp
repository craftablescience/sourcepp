#include <sndpp/XWV.h>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sndpp;
using namespace sourcepp;

XWV::XWV(std::span<const std::byte> xwvData) {
	BufferStreamReadOnly stream{xwvData.data(), xwvData.size()};
	if (stream.read<uint32_t>() != XWV_SIGNATURE) {
		return;
	}

	stream.set_big_endian(true);
	if (stream.read<uint32_t>() != 4) {
		return;
	}
	stream.skip<uint32_t>(2);

	const auto dataOffset = stream.read<uint32_t>();
	const auto dataLength = stream.read<uint32_t>();
	this->data = stream.at_bytes(dataLength, dataOffset);

	stream >> this->decodedSampleCount >> this->loopStart >> this->loopBlock >> this->leadingSampleCount >> this->trailingSampleCount;
	stream.skip<uint16_t>();

	stream >> this->format >> this->bitsPerSample >> this->frequency >> this->channelCount >> this->quality;

	this->opened = true;
}

XWV::XWV(const std::string& xwvPath)
		: XWV(fs::readFileBuffer(xwvPath)) {}

XWV::operator bool() const {
	return this->opened;
}

const std::vector<std::byte>& XWV::getData() const {
	return this->data;
}

uint32_t XWV::getDecodedSampleCount() const {
	return this->decodedSampleCount;
}

int32_t XWV::getLoopStart() const {
	return this->loopStart;
}

uint16_t XWV::getLoopBlock() const {
	return this->loopBlock;
}

uint16_t XWV::getLeadingSampleCount() const {
	return this->leadingSampleCount;
}

uint16_t XWV::getTrailingSampleCount() const {
	return this->trailingSampleCount;
}

XWV::Format XWV::getFormat() const {
	return this->format;
}

uint8_t XWV::getBitsPerSample() const {
	return this->bitsPerSample;
}

XWV::Frequency XWV::getFrequency() const {
	return this->frequency;
}

uint8_t XWV::getChannelCount() const {
	return this->channelCount;
}

uint8_t XWV::getQuality() const {
	return this->quality;
}
