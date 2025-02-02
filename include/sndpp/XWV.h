#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <sourcepp/parser/Binary.h>

namespace sndpp {

constexpr auto XWV_SIGNATURE = sourcepp::parser::binary::makeFourCC("XWV ");

class XWV {
public:
	enum class Frequency : uint8_t {
		HZ_11025,
		HZ_22050,
		HZ_44100,
	};

	enum class Format : uint8_t {
		PCM,
		XMA,
		ADPCM,
		MP3,
	};

	explicit XWV(std::span<const std::byte> xwvData);

	explicit XWV(const std::string& xwvPath);

	explicit operator bool() const;

	[[nodiscard]] const std::vector<std::byte>& getData() const;

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
	std::vector<std::byte> data;
	uint32_t decodedSampleCount{};
	int32_t loopStart{};
	uint16_t loopBlock{};
	uint16_t leadingSampleCount{};
	uint16_t trailingSampleCount{};
	Format format{};
	uint8_t bitsPerSample{};
	Frequency frequency{};
	uint8_t channelCount{};
	uint8_t quality{};

	bool opened = false;
};

} // namespace sndpp
