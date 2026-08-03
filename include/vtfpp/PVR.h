#pragma once

#include <array>
#include <filesystem>

#include <sourcepp/parser/Binary.h>
#include <sourcepp/Macros.h>
#include <sourcepp/Math.h>

#include "ImageConversion.h"

namespace vtfpp {

constexpr uint32_t GBIX_SIGNATURE = sourcepp::parser::binary::makeFourCC("GBIX");
constexpr uint32_t PVR_SIGNATURE = sourcepp::parser::binary::makeFourCC("PVRT");

class PVR {
public:
	enum ImageType : uint8_t {
		IMAGE_TYPE_NONE = 0,
		IMAGE_TYPE_TWIDDLED = 1,
		IMAGE_TYPE_TWIDDLED_MM = 2,
		IMAGE_TYPE_VQ = 3,
		IMAGE_TYPE_VQ_MM = 4,
		IMAGE_TYPE_PAL4 = 5,
		IMAGE_TYPE_PAL4_MM = 6,
		IMAGE_TYPE_PAL8 = 7,
		IMAGE_TYPE_PAL8_MM = 8,
		IMAGE_TYPE_RECTANGULAR = 9,
		IMAGE_TYPE_RECTANGULAR_MM = 10,
		IMAGE_TYPE_STRIDE = 11,
		IMAGE_TYPE_STRIDE_MM = 12,
		IMAGE_TYPE_RECTANGULAR_TWIDDLED = 13,
		IMAGE_TYPE_BMP = 14,
		IMAGE_TYPE_BMP_MM = 15,
		IMAGE_TYPE_SMALLVQ = 16,
		IMAGE_TYPE_SMALLVQ_MM = 17
	};
	enum PixelType : uint8_t {
		PIXEL_TYPE_ARGB1555 = 0,
		PIXEL_TYPE_RGB565 = 1,
		PIXEL_TYPE_ARGB4444 = 2,
		PIXEL_TYPE_YUV422 = 3,
		PIXEL_TYPE_BUMP = 4,
		PIXEL_TYPE_RGB555 = 5,
		PIXEL_TYPE_YUV420 = 6
	};

	explicit PVR(std::span<const std::byte> pvrData);

	explicit PVR(const std::filesystem::path& pvrPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint16_t getWidth() const;

	[[nodiscard]] uint16_t getHeight() const;

	[[nodiscard]] bool hasImageData() const;

	[[nodiscard]] bool hasMips() const;

	[[nodiscard]] bool isTwiddled() const;

	[[nodiscard]] bool isPaletted() const;

	[[nodiscard]] ImageFormat getFormat() const;

	[[nodiscard]] uint8_t getMipCount() const;

	[[nodiscard]] std::span<const std::byte> getImageDataRaw(uint8_t mip = 0) const;

	[[nodiscard]] std::span<std::byte> getImageDataRaw(uint8_t mip = 0);

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint8_t mip = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsRGBA8888(uint8_t mip = 0) const;

protected:
	std::span<std::byte> imageData;
	uint16_t width = 0;
	uint16_t height = 0;
	ImageType imageType = IMAGE_TYPE_NONE;
	PixelType pixelType = PIXEL_TYPE_ARGB1555;
	ImageFormat format = ImageFormat::EMPTY;
	uint8_t mipCount = 1;
};

} // namespace vtfpp
