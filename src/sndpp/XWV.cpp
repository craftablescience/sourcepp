// ReSharper disable CppRedundantQualifier

#include <sndpp/XWV.h>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sndpp;
using namespace sourcepp;

namespace {

/// Lifted from <a href="https://github.com/vgmstream/vgmstream/blob/86ef4c768977271616c309964c6f600389030e5f/src/coding/ima_decoder.c#L1396">vgmstream</a> with minor alterations
[[nodiscard]] constexpr uint64_t xmaBytesToSamples(uint64_t bytes, uint8_t channels) {
	const auto blockAlign = channels * 0x24;
	const auto mod = bytes % blockAlign;
	return bytes / blockAlign * (blockAlign - 4 * channels) * 2 / channels + (mod > 0 && mod > 0x04 * channels ? (mod - 0x04*channels) * 2 / channels : 0);
}

} // namespace

XWV::XWV(std::span<const std::byte> xwvData)
		: version{Version::V0}
		, loopStart{-1}
		, format{Format::PCM} {
	BufferStreamReadOnly stream{xwvData.data(), xwvData.size()};

	switch (stream >> this->version; this->version) {
		case Version::V0: {
			const auto dataLength = stream.read<uint32_t>();
			const auto dataOffset = stream.read<uint32_t>();
			this->audioData = stream.at_bytes(dataLength, dataOffset);

			stream >> this->loopStart;

			if (const auto valveDataLength = stream.read<uint16_t>()) {
				this->valveData = stream.at_bytes(valveDataLength, sizeof(uint32_t) * 3 + sizeof(int32_t) + sizeof(uint8_t) * 4);
			}

			stream >> this->format;

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
				this->decodedSampleCount = this->audioData.size() / this->channelCount / sizeof(int16_t);
			} else {
				this->bitsPerSample = 4;
				this->decodedSampleCount = ::xmaBytesToSamples(this->audioData.size(), this->channelCount);
			}
			break;
		}
		case Version::V1: {
			const auto headerSize = stream.read<uint32_t>();
			if (headerSize != sizeof(uint32_t) * 4 + sizeof(int32_t) + sizeof(uint16_t) + sizeof(uint8_t) * 4) {
				return;
			}

			const auto dataLength = stream.read<uint32_t>();
			const auto dataOffset = stream.read<uint32_t>();
			this->audioData = stream.at_bytes(dataLength, dataOffset);

			stream >> this->loopStart;

			if (const auto valveDataLength = stream.read<uint16_t>()) {
				this->valveData = stream.at_bytes(valveDataLength, headerSize);
			}

			stream >> this->format >> this->bitsPerSample;

			const auto packed = stream.read<uint8_t>();
			this->frequency = static_cast<Frequency>(packed & 0x0f);
			this->channelCount = packed >> 4;

			// Fill in new values
			this->leadingSampleCount = 0;
			this->trailingSampleCount = 0;
			this->quality = 63; // useless?

			this->loopBlock = 0; // used in XMA2
			if (this->format == Format::PCM) {
				this->decodedSampleCount = this->audioData.size() / this->channelCount / sizeof(int16_t);
			} else {
				this->decodedSampleCount = ::xmaBytesToSamples(this->audioData.size(), this->channelCount);
			}
			break;
		}
		case Version::V4: {
			stream.set_big_endian(true);
			if (stream.read<uint32_t>() != 4) {
				return;
			}

			const auto headerSize = stream.read<uint32_t>();
			if (headerSize != sizeof(uint32_t) * 7 + sizeof(int32_t) + sizeof(uint16_t) * 4 + sizeof(uint8_t) * 8) {
				return;
			}

			const auto staticDataSize = stream.read<uint32_t>();
			this->staticData = stream.at_bytes(staticDataSize, headerSize);

			const auto dataOffset = stream.read<uint32_t>();
			const auto dataLength = stream.read<uint32_t>();
			this->audioData = stream.at_bytes(dataLength, dataOffset);

			stream >> this->decodedSampleCount >> this->loopStart >> this->loopBlock >> this->leadingSampleCount >> this->trailingSampleCount;

			if (const auto valveDataLength = stream.read<uint16_t>()) {
				this->valveData = stream.at_bytes(valveDataLength, headerSize);
			}

			stream >> this->format;
			if (this->format == Format::XMA) {
				// This format uses the same index as XMA, we need to remap it so the enum entries are different here
				this->format = Format::XMA2;
			}

			stream >> this->bitsPerSample >> this->frequency >> this->channelCount >> this->quality;

			if (stream.read<uint8_t>()) {
				this->seekTable = stream.read_bytes(this->audioData.size() * sizeof(uint32_t) / 2048);
			}
			break;
		}
	}
	this->opened = true;
}

XWV::XWV(const std::string& xwvPath)
		: XWV(fs::readFileBuffer(xwvPath)) {}

XWV::operator bool() const {
	return this->opened;
}

XWV::Version XWV::getVersion() const {
	return this->version;
}

const std::vector<std::byte>& XWV::getAudioDataRaw() const {
	return this->audioData;
}

const std::vector<std::byte>& XWV::getStaticData() const {
	return this->staticData;
}

const std::vector<std::byte>& XWV::getValveData() const {
	return this->valveData;
}

const std::vector<std::byte>& XWV::getSeekTableData() const {
	return this->seekTable;
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
