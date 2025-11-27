#pragma once

#include <span>
#include <string>
#include <utility>
#include <vector>

#include <sourcepp/parser/Binary.h>

namespace sndpp {

constexpr auto RIFF_SIGNATURE = sourcepp::parser::binary::makeFourCC("RIFF");
constexpr auto RIFX_SIGNATURE = sourcepp::parser::binary::makeFourCC("RIFX");
constexpr auto FFIR_SIGNATURE = sourcepp::parser::binary::makeFourCC("FFIR");

class RIFF {
public:
	explicit RIFF(uint32_t type, uint32_t signature = RIFF_SIGNATURE);

	explicit RIFF(std::vector<std::byte>&& riffData, uint32_t type_);

	explicit RIFF(std::span<const std::byte> riffData, uint32_t type_);

	explicit RIFF(const std::string& riffPath, uint32_t type_);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getSignature() const;

	[[nodiscard]] bool isBigEndian() const;

	[[nodiscard]] const std::vector<std::pair<uint32_t, std::span<std::byte>>>& getChunks() const;

	[[nodiscard]] bool hasChunk(uint32_t chunkType) const;

	[[nodiscard]] std::vector<std::byte> getFirstChunk(uint32_t chunkType) const;

	[[nodiscard]] bool hasNthChunk(uint32_t chunkType, uint32_t n) const;

	[[nodiscard]] std::vector<std::byte> getNthChunk(uint32_t chunkType, uint32_t n) const;

protected:
	uint32_t signature;
	uint32_t type;
	std::vector<std::pair<uint32_t, std::span<std::byte>>> chunks;

	std::vector<std::byte> data;
};

} // namespace sndpp
