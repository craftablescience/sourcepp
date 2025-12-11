#pragma once

#include <filesystem>
#include <span>
#include <vector>

#include <sourcepp/parser/Binary.h>

namespace sndpp {

class XWV {
public:
	enum class Version : uint32_t {
		V0 = 20, // This is actually just header size, but it's a good identifier
		V1 = sourcepp::parser::binary::makeFourCC("XWV1"), // Seemingly not present in any actual releases but present in the 2006 SDK release
		V4 = sourcepp::parser::binary::makeFourCC("XWV "), // Used on X360 and PS3 with no variation
	};

	enum class Format : uint8_t {
		PCM  = 0, // PCM16LE (v0+)
		XMA  = 1, // Xbox IMA-ADPCM (v0+)
		XMA2 = 2, // XMA2 (v4+) - note this slot is technically ADPCM, but it doesn't work in any official release, so it's been repurposed and XMA is remapped to XMA2 when parsing v4
		MP3  = 3, // MP3 (v4+)
	};

	enum class Frequency : uint8_t {
		HZ_11025 = 0,
		HZ_22050 = 1,
		HZ_44100 = 2,
	};

	explicit XWV(std::span<const std::byte> xwvData);

	explicit XWV(const std::filesystem::path& xwvPath);

	explicit operator bool() const;

	[[nodiscard]] Version getVersion() const;

	[[nodiscard]] const std::vector<std::byte>& getAudioDataRaw() const;

	[[nodiscard]] const std::vector<std::byte>& getStaticData() const;

	[[nodiscard]] const std::vector<std::byte>& getValveData() const;

	[[nodiscard]] const std::vector<std::byte>& getSeekTableData() const;

	[[nodiscard]] uint32_t getDecodedSampleCount() const;

	[[nodiscard]] int32_t getLoopStart() const;

	[[nodiscard]] uint16_t getLoopBlock() const;

	[[nodiscard]] uint16_t getLeadingSampleCount() const;

	[[nodiscard]] uint16_t getTrailingSampleCount() const;

	[[nodiscard]] Format getFormat() const;

	[[nodiscard]] uint8_t getBitsPerSample() const;

	[[nodiscard]] Frequency getFrequency() const;

	[[nodiscard]] uint8_t getChannelCount() const;

	[[nodiscard]] uint8_t getQuality() const;

protected:
	std::vector<std::byte> audioData;
	std::vector<std::byte> staticData;
	std::vector<std::byte> valveData;
	std::vector<std::byte> seekTable;

	Version version;
	uint32_t decodedSampleCount;
	int32_t loopStart;
	uint16_t loopBlock;
	uint16_t leadingSampleCount;
	uint16_t trailingSampleCount;
	Format format;
	uint8_t bitsPerSample;
	Frequency frequency;
	uint8_t channelCount;
	uint8_t quality;

	bool opened = false;
};

} // namespace sndpp
