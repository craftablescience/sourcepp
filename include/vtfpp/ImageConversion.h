#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "ImageFormats.h"

namespace vtfpp::ImageConversion {

/// Converts an image from one format to another.
[[nodiscard]] std::vector<std::byte> convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height);

/// Converts several images from one format to another.
[[nodiscard]] std::vector<std::byte> convertSeveralImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount);

/// Converts image data to a PNG or HDR file. HDR output will be used for floating-point formats.
[[nodiscard]] std::vector<std::byte> convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height);

enum class ResizeEdge {
	// Matches stbir_edge
	CLAMP = 0,
	REFLECT,
	WRAP,
	ZERO,
};

enum class ResizeFilter {
	// Matches stbir_filter
	DEFAULT = 0,
	BOX,
	BILINEAR,
	CUBIC_BSPLINE,
	CATMULLROM,
	MITCHELL,
	POINT_SAMPLE,
};

enum class ResizeMethod {
	NONE,
	POWER_OF_TWO_BIGGER,
	POWER_OF_TWO_SMALLER,
	POWER_OF_TWO_NEAREST,
};

/// Get the new image size given a resize method
[[nodiscard]] uint16_t getResizedDim(uint16_t n, ResizeMethod method);

/// Set the new image dimensions given a resize method
void setResizedDims(uint16_t& width, ResizeMethod widthResize, uint16_t& height, ResizeMethod heightResize);

/// Resize given image data to the new dimensions
[[nodiscard]] std::vector<std::byte> resizeImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Resize given image data to the new dimensions, where the new width and height are governed by the resize methods
[[nodiscard]] std::vector<std::byte> resizeImageDataStrict(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t& widthOut, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, uint16_t& heightOut, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

} // namespace vtfpp::ImageConversion
