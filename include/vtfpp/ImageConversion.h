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

/// Converts an HDRI into six cubemap (or skybox) faces. The output image data is in the same image format as the input.
/// If in cubemap mode, the output images have the following order: front, back, left, right, down, up.
/// If in skybox mode, the output images have the following order: right, left, back, front, up, down.
/// Resolution is the output size (width, height) of each image slice. 0 leaves it at the input size, the height of the HDRI.
/// Fails (returns empty vectors) if the input data is empty, the given width is not 2x the height, or an error was encountered.
[[nodiscard]] std::array<std::vector<std::byte>, 6> convertHDRIToCubeMap(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, uint16_t resolution = 0, bool bilinear = true, bool skybox = false);

/// Takes in RGBA32323232F format image data, returns SOURCEPP_BGRA8888_HDR compressed HDR image data (alias for BGRA8888)
[[nodiscard]] std::vector<std::byte> compressBGRA8888HDR(std::span<const std::byte> imageData, float overbrightFactor = 16.f);

/// Takes in SOURCEPP_BGRA8888_HDR compressed HDR image data (alias for BGRA8888), returns RGBA32323232F format image data
[[nodiscard]] std::vector<std::byte> decompressBGRA8888HDR(std::span<const std::byte> imageData, float overbrightFactor = 16.f);

/// Takes in RGBA32323232F format image data, returns SOURCEPP_RGBA16161616_HDR compressed HDR image data (alias for RGBA16161616)
[[nodiscard]] std::vector<std::byte> compressRGBA16161616HDR(std::span<const std::byte> imageData, bool flipExponentAndSignificand = false);

/// Takes in SOURCEPP_RGBA16161616_HDR compressed HDR image data (alias for RGBA16161616), returns RGBA32323232F format image data
[[nodiscard]] std::vector<std::byte> decompressRGBA16161616HDR(std::span<const std::byte> imageData, bool flipExponentAndSignificand = false);

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
[[nodiscard]] std::vector<std::byte> resizeImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, bool premultipliedAlpha, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Resize given image data to the new dimensions, where the new width and height are governed by the resize methods
[[nodiscard]] std::vector<std::byte> resizeImageDataStrict(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t& widthOut, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, uint16_t& heightOut, ResizeMethod heightResize, bool srgb, bool premultipliedAlpha, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Crops the given image to the new dimensions. If the image format is compressed it will be converted to its container format before the crop, and converted back before returning
[[nodiscard]] std::vector<std::byte> cropImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset);

/// Pad the given image with pixels that are the same color as the edge. Padding is applied to the right and bottom of the image.
[[nodiscard]] std::vector<std::byte> padImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t widthPad, uint16_t height, uint16_t heightPad);

/// Perform gamma correction on the given image data. Will not perform gamma correction if the input image format is large, console, P8, A8, UV88, UVLX8888, or UVWQ8888
void gammaCorrectImageData(std::span<std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, float gamma);

/// Invert the green channel on the given image data. Meant for converting normal maps between OpenGL and DirectX formats
void invertGreenChannelForImageData(std::span<std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height);

/// Perform Hable tonemapping on the given image data
void hableTonemapImageData(std::span<std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height);

enum class DistanceDither {
	NONE = 0, ///< No dithering.
	GRADIENT_TANGENT, ///< Experimental dithering approach that diffuses quantization error perpendicular to the distance gradient at any destination pixel. Slow and requires up to double the memory.
	// anything generic that doesn't depend on the geometry of the distance field (floyd-steinberg etc) would belong somewhere more general.
};

enum class DistanceFlags : uint32_t {
	NONE = 0,
	DISTANCEAA     = 1 << 0, ///< Experimental; interpret the alpha channel as antialiased. Can result in a more precise distance map, but produces nonsense if large gradients are present.
	EUCLIDEAN      = 1 << 1, ///< The distance-mapping algorithm is a brute-force scan of a *square* area. If this is enabled, only accept distance hits in a circular area.
	SAMPLECENTERED = 1 << 2, ///< Search from the center of pixels (in destination coordinate space) rather than in their north-west corners. Can mitigate a perceived southeast shift at extreme reductions.
};

SOURCEPP_BITFLAGS_ENUM(DistanceFlags)

/// In one operation, convert an image's alpha channel, or, for single-channel formats, its only channel, to a VTEX-style distance map, and downscale other channels, if present in the output, according to the given resize parameters.
/// @return Empty if:
/// * \p inFormat or \p outFormat do not satisy their documented predicates,
/// * \p reduceX or \p reduceY are not powers of two,
/// * \p distanceSpread would result in a search radius of zero with either \p reduceX or \p reduceY,
/// * \p alphaThreshold is out of range, or
/// * \p edge is ResizeEdge::ZERO.
///
/// Otherwise, an image at \p width / \p reduceX by \p height / \p reduceY in \p outFormat:
/// * if \p outFormat is single-channel, R contains the distance map.
/// * if \p outFormat has alpha, A contains the distance map, while R/G/B contain:
///   * if \p inFormat had R/G/B, the scaled R/G/B of the input image, otherwise
///   * 0 in all other channels.
[[nodiscard]] std::vector<std::byte> alphaToDistance(
	std::span<const std::byte> imageData,
	ImageFormat inFormat, ///< Any format that is either single-channel, or has an alpha channel.
	ImageFormat outFormat, ///< The same requirements as inFormat; channel count does not need to correspond to inFormat.
	uint16_t width,
	uint16_t height,
	uint16_t reduceX, ///< Power-of-two horizontal reduction factor.
	uint16_t reduceY, ///< Power-of-two vertical reduction factor.
	bool srgb, ///< Used in the case of RGBA->RGBA. premultipliedAlpha is omitted.
	float distanceSpread = 1.0f, ///< Scale factor beyond \p reduceX and \p reduceY, of search radius for distance hits.
	float alphaThreshold = 0.04f, ///< Threshold below which alpha values are considered zero.
	DistanceFlags flags = DistanceFlags::NONE, ///< Additional behaviors that deviate from VTEX but may be useful; see DistanceFlags.
	DistanceDither dither = DistanceDither::NONE, ///< Internally, distance maps are always computed in floating point. When set to a value other than NONE, and the output format is of integral type, dithering is applied prior to quantization. Depending on the application and contents, this can improve or worsen the quality of the distance map.
	ResizeFilter filter = ResizeFilter::NICE, ///< Default value mimics VTEX. Does not affect the alpha channel; distance mapping is a distinct sampling operation from source to destination space. Entirely unused if input is single-channel.
	ResizeEdge edge = ResizeEdge::CLAMP, ///< Dictates the sampling policy of the distance function regardless of whether there are non-alpha channels to be resized.
	bool *valveQuirks = nullptr ///< When non-null, mimic VTEX's policy of blanking out any edge pixels in the distance map to 0 (i.e. infinite distance), and report back whether this resulted in any change (such that a command-line tool emulating VTEX would want to report a warning).
);

} // namespace vtfpp::ImageConversion
