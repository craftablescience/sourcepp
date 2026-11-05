#pragma once

#include <array>
#include <filesystem>

#include <sourcepp/parser/Binary.h>
#include <sourcepp/Macros.h>
#include <sourcepp/Math.h>

#include "ImageConversion.h"

namespace vtfpp {

constexpr uint32_t DDS_SIGNATURE = sourcepp::parser::binary::makeFourCC("DDS ");

class DDS {
public:
	struct CreationOptions {
		ImageFormat outputFormat = FORMAT_DEFAULT;
		float compressedFormatQuality = ImageConversion::DEFAULT_COMPRESSED_QUALITY;
		ImageConversion::ResizeMethod widthResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeMethod heightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT;
		ImageConversion::ResizeBounds resizeBounds;
		bool isCubeMap = false;
		uint16_t initialDepth = 1;
		bool computeMips = true;
	};

	/// This value is only valid when passed to DDS::create through CreationOptions
	static constexpr auto FORMAT_UNCHANGED = static_cast<ImageFormat>(-2);

	/// This value is only valid when passed to DDS::create through CreationOptions or DDS::setFormat
	static constexpr auto FORMAT_DEFAULT = static_cast<ImageFormat>(-1);

	DDS();

	explicit DDS(std::vector<std::byte>&& ddsData);

	explicit DDS(std::span<const std::byte> ddsData);

	explicit DDS(const std::filesystem::path& ddsPath);

	DDS(const DDS& other);

	DDS& operator=(const DDS& other);

	DDS(DDS&&) noexcept = default;

	DDS& operator=(DDS&&) noexcept = default;

	[[nodiscard]] explicit operator bool() const;

	static bool create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::filesystem::path& ddsPath, const CreationOptions& options);

	static bool create(ImageFormat format, uint16_t width, uint16_t height, const std::filesystem::path& ddsPath, const CreationOptions& options);

	[[nodiscard]] static DDS create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options);

	[[nodiscard]] static DDS create(ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options);

	static bool create(const std::filesystem::path& imagePath, const std::filesystem::path& ddsPath, const CreationOptions& options);

	[[nodiscard]] static DDS create(const std::filesystem::path& imagePath, const CreationOptions& options);

	[[nodiscard]] ImageConversion::ResizeMethod getImageWidthResizeMethod() const;

	[[nodiscard]] ImageConversion::ResizeMethod getImageHeightResizeMethod() const;

	void setImageResizeMethods(ImageConversion::ResizeMethod imageWidthResizeMethod_, ImageConversion::ResizeMethod imageHeightResizeMethod_);

	void setImageWidthResizeMethod(ImageConversion::ResizeMethod imageWidthResizeMethod_);

	void setImageHeightResizeMethod(ImageConversion::ResizeMethod imageHeightResizeMethod_);

	[[nodiscard]] uint16_t getWidth(uint8_t mip = 0) const;

	[[nodiscard]] uint16_t getPaddedWidth(uint8_t mip = 0) const;

	[[nodiscard]] uint16_t getHeight(uint8_t mip = 0) const;

	[[nodiscard]] uint16_t getPaddedHeight(uint8_t mip = 0) const;

	[[nodiscard]] bool hasImageData() const;

	[[nodiscard]] ImageFormat getFormat() const;

	[[nodiscard]] uint8_t getMipCount() const;

	[[nodiscard]] std::span<const std::byte> getImageDataRaw(uint8_t mip = 0) const;

	[[nodiscard]] std::span<std::byte> getImageDataRaw(uint8_t mip = 0);

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint8_t mip = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsRGBA8888(uint8_t mip = 0) const;

	bool setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	bool setImage(const std::filesystem::path& imagePath, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY, ImageConversion::ResizeBounds resizeBounds = {});

protected:
	std::vector<std::byte> data;
	std::span<std::byte> imageData;
	uint16_t width = 0;
	uint16_t height = 0;
	uint16_t depth = 1;
	ImageFormat format = ImageFormat::EMPTY;
	uint8_t mipCount = 1;
	ImageConversion::ResizeMethod imageWidthResizeMethod  = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
	ImageConversion::ResizeMethod imageHeightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
	bool opened = false;
};

} // namespace vtfpp
