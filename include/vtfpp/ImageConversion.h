#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <vector>

#include <BufferStream.h>

#include "ImageFormats.h"

namespace vtfpp::ImageConversion {

// Negative means use default quality
// BC7, BC6H: 0.1
// Everything else: 1.0
constexpr float DEFAULT_COMPRESSED_QUALITY = -1.f;

/// Converts an image from one format to another.
[[nodiscard]] std::vector<std::byte> convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height, float quality = DEFAULT_COMPRESSED_QUALITY);

/// Converts several images from one format to another.
[[nodiscard]] std::vector<std::byte> convertSeveralImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth, float quality = DEFAULT_COMPRESSED_QUALITY);

/// Converts an HDRI into a cubemap. The output image data is in the same image format as the input.
/// The output images have the following order: front, back, left, right, down, up.
/// Resolution is the output size (width, height) of each image slice. 0 leaves it at the input size, the height of the HDRI.
/// Fails (returns empty vectors) if the input data is empty, the given width is not 2x the height, or an error was encountered.
[[nodiscard]] std::array<std::vector<std::byte>, 6> convertHDRIToCubeMap(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, uint16_t resolution = 0, bool bilinear = true);

enum class FileFormat {
	DEFAULT = 0,
	PNG     = 1,
	JPG     = 2,
	BMP     = 3,
	TGA     = 4,
#ifdef VTFPP_SUPPORT_WEBP
	WEBP    = 5,
#endif
#ifdef VTFPP_SUPPORT_QOI
	QOI     = 6,
#endif
	HDR     = 7,
#ifdef VTFPP_SUPPORT_EXR
	EXR     = 8,
#endif
};

/// PNG for integer formats, EXR for floating point formats (or HDR if EXR support is disabled)
[[nodiscard]] FileFormat getDefaultFileFormatForImageFormat(ImageFormat format);

/// Converts image data to the given file format (PNG or EXR by default).
[[nodiscard]] std::vector<std::byte> convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat = FileFormat::DEFAULT);

[[nodiscard]] std::vector<std::byte> convertFileToImageData(std::span<const std::byte> fileData, ImageFormat& format, int& width, int& height, int& frameCount);

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
	CATMULL_ROM,
	MITCHELL,
	POINT_SAMPLE,

	// User-defined
	KAISER = 100,
	/// Valve NICE filtering, equivalent to Lanczos-3
	NICE = 101,
};

enum class ResizeMethod {
	NONE,
	POWER_OF_TWO_BIGGER,
	POWER_OF_TWO_SMALLER,
	POWER_OF_TWO_NEAREST,
};

struct ResizeBounds {
	uint16_t resizeMinWidth = 0;
	uint16_t resizeMaxWidth = 0;
	uint16_t resizeMinHeight = 0;
	uint16_t resizeMaxHeight = 0;

	ResizeBounds() = default;
	explicit ResizeBounds(uint16_t size) : resizeMinWidth{size}, resizeMaxWidth{size}, resizeMinHeight{size}, resizeMaxHeight{size} {}
	ResizeBounds(uint16_t width, uint16_t height) : resizeMinWidth{width}, resizeMaxWidth{width}, resizeMinHeight{height}, resizeMaxHeight{height} {}
	ResizeBounds(uint16_t minWidth, uint16_t maxWidth, uint16_t minHeight, uint16_t maxHeight) : resizeMinWidth{minWidth}, resizeMaxWidth{maxWidth}, resizeMinHeight{minHeight}, resizeMaxHeight{maxHeight} {}

	[[nodiscard]] std::pair<uint16_t, uint16_t> clamp(uint16_t width, uint16_t height) const;
};

/// Get the new image size given a resize method
[[nodiscard]] uint16_t getResizedDim(uint16_t n, ResizeMethod method);

/// Set the new image dimensions given a resize method
void setResizedDims(uint16_t& width, ResizeMethod widthResize, uint16_t& height, ResizeMethod heightResize);

/// Resize given image data to the new dimensions
[[nodiscard]] std::vector<std::byte> resizeImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Resize given image data to the new dimensions, where the new width and height are governed by the resize methods
[[nodiscard]] std::vector<std::byte> resizeImageDataStrict(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t& widthOut, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, uint16_t& heightOut, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Crops the given image to the new dimensions. If the image format is compressed it will be converted to its container format before the crop, and converted back before returning
[[nodiscard]] std::vector<std::byte> cropImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset);

/// Pad the given image with pixels that are the same color as the edge. Padding is applied to the right and bottom of the image.
[[nodiscard]] std::vector<std::byte> padImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t widthPad, uint16_t height, uint16_t heightPad);

/// Perform gamma correction on the given image data. Will not perform gamma correction if the input image format is large, console, P8, A8, UV88, UVLX8888, or UVWQ8888
void gammaCorrectImageData(std::span<std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, float gamma);

/// Invert the green channel. Meant for converting normal maps between OpenGL and DirectX formats
void invertGreenChannelForImageData(std::span<std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height);

} // namespace vtfpp::ImageConversion
