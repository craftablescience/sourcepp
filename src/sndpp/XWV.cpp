// ReSharper disable CppRedundantQualifier

#include <sndpp/XWV.h>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sndpp;
using namespace sourcepp;

namespace {

/// Lifted from <a href="https://github.com/vgmstream/vgmstream/blob/86ef4c768977271616c309964c6f600389030e5f/src/coding/ima_decoder.c#L1396">vgmstream</a> with minor alterations
[[nodiscard]] constexpr uint64_t xmaBytesToSamples(uint64_t bytes, uint8_t channels) {
	const int blockAlign = channels * 0x24;
	const int mod = bytes % blockAlign;
	return bytes / blockAlign * (blockAlign - 4 * channels) * 2 / channels + (mod > 0 && mod > 0x04 * channels ? (mod - 0x04*channels) * 2 / channels : 0);
}

} // namespace

XWV::XWV(std::span<const std::byte> xwvData) {
	BufferStreamReadOnly stream{xwvData.data(), xwvData.size()};

	stream >> this->platform;
	if (this->platform == Platform::XBOX) {
		// Xbox's "magic" is actually its header size!
		const auto dataLength = stream.read<uint32_t>();
		const auto dataOffset = stream.read<uint32_t>();
		this->data = stream.at_bytes(dataLength, dataOffset);

		stream >> this->loopStart >> this->format;

		const auto packed = stream.read<uint8_t>();
		this->frequency = static_cast<Frequency>(packed & 0x0f);
		this->channelCount = packed >> 4;

		// Fill in new values
		this->leadingSampleCount = 0;
		this->trailingSampleCount = 0;
		this->quality = 63; // useless?

		this->loopBlock = 0; // used in XMA2
		if (this->format == Format::PCM) {
			this->bitsPerSample = 16;
			this->decodedSampleCount = this->data.size() / this->channelCount / sizeof(int16_t);
		} else {
			this->bitsPerSample = 4;
			this->decodedSampleCount = ::xmaBytesToSamples(this->data.size(), this->channelCount);
		}

		this->opened = true;
		return;
	}
	if (this->platform != Platform::X360_AND_PS3) {
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

	// todo(sndpp): add support for seek table, VDAT section
	this->opened = true;
}

XWV::XWV(const std::string& xwvPath)
		: XWV(fs::readFileBuffer(xwvPath)) {}

XWV::operator bool() const {
	return this->opened;
}

XWV::Platform XWV::getPlatform() const {
	return this->platform;
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
