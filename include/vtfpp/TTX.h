#pragma once

#include <tuple>

#include "VTF.h"

namespace vtfpp {

constexpr uint32_t TTH_SIGNATURE = sourcepp::parser::binary::makeFourCC("TTH\0");

class TTX {
public:
	explicit TTX(std::span<const std::byte> tthData, std::span<const std::byte> ttzData = {});

	explicit TTX(const std::string& tthPath, const std::string& ttzPath = "");

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint8_t getMajorVersion() const;

	[[nodiscard]] uint8_t getMinorVersion() const;

	void setVersion(uint8_t newMajorVersion, uint8_t newMinorVersion);

	void setMajorVersion(uint8_t newMajorVersion);

	void setMinorVersion(uint8_t newMinorVersion);

	[[nodiscard]] uint8_t getAspectRatioType() const;

	void setAspectRatioType(uint8_t newAspectRatioType);

	[[nodiscard]] const std::vector<uint64_t>& getMipFlags() const;

	[[nodiscard]] std::vector<uint64_t>& getMipFlags();

	[[nodiscard]] const VTF& getVTF() const;

	[[nodiscard]] VTF& getVTF();

	void setCompressionLevel(int16_t newCompressionLevel);

	[[nodiscard]] std::pair<std::vector<std::byte>, std::vector<std::byte>> bake() const;

	bool bake(const std::string& tthPath, const std::string& ttzPath) const;

protected:
	bool opened = false;
	int16_t compressionLevel = -1;

	VTF vtf;

	uint8_t majorVersion{};
	uint8_t minorVersion{};
	uint8_t aspectRatioType{};
	std::vector<uint64_t> mipFlags;
};

} // namespace vtfpp
