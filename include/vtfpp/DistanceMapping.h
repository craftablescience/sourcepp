#pragma once

#include <vtfpp/ImageConversion.h>

#include <vector>

#include <sourcepp/Macros.h>

namespace vtfpp::DistanceMapping {

enum class Dither {
	NONE = 0, ///< No dithering.
	GRADIENT_TANGENT, ///< Experimental dithering approach that diffuses quantization error perpendicular to the distance gradient at any destination pixel. Slow and requires up to double the memory.
	// anything generic that doesn't depend on the geometry of the distance field (floyd-steinberg etc) would belong somewhere more general.
};

enum class Flags : uint32_t {
	NONE = 0,
	DISTANCEAA     = 1 << 0, ///< Experimental; interpret the alpha channel as antialiased. Can result in a more precise distance map, but produces nonsense if large gradients are present.
	EUCLIDEAN      = 1 << 1, ///< The distance-mapping algorithm is a brute-force scan of a *square* area. If this is enabled, only accept distance hits in a circular area.
	SAMPLECENTERED = 1 << 2, ///< Search from the center of pixels (in destination coordinate space) rather than in their north-west corners. Can mitigate a perceived southeast shift at extreme reductions.
};

SOURCEPP_BITFLAGS_ENUM(Flags)

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
	vtfpp::ImageFormat inFormat, ///< Any format that is either single-channel, or has an alpha channel.
	vtfpp::ImageFormat outFormat, ///< The same requirements as inFormat; channel count does not need to correspond to inFormat.
	uint16_t width,
	uint16_t height,
	uint16_t reduceX, ///< Power-of-two horizontal reduction factor.
	uint16_t reduceY, ///< Power-of-two vertical reduction factor.
	bool srgb, ///< Used in the case of RGBA->RGBA. premultipliedAlpha is omitted.
	float distanceSpread = 1.0f, ///< Scale factor beyond \p reduceX and \p reduceY, of search radius for distance hits.
	float alphaThreshold = 0.04f, ///< Threshold below which alpha values are considered zero.
	Flags flags = Flags::NONE, ///< Additional behaviors that deviate from VTEX but may be useful; see DistanceFlags.
	Dither dither = Dither::NONE, ///< Internally, distance maps are always computed in floating point. When set to a value other than NONE, and the output format is of integral type, dithering is applied prior to quantization. Depending on the application and contents, this can improve or worsen the quality of the distance map.
	ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::NICE, ///< Default value mimics VTEX. Does not affect the alpha channel; distance mapping is a distinct sampling operation from source to destination space. Entirely unused if input is single-channel.
	ImageConversion::ResizeEdge edge = ImageConversion::ResizeEdge::CLAMP, ///< Dictates the sampling policy of the distance function regardless of whether there are non-alpha channels to be resized.
	bool *valveQuirks = nullptr ///< When non-null, mimic VTEX's policy of blanking out any edge pixels in the distance map to 0 (i.e. infinite distance), and report back whether this resulted in any change (such that a command-line tool emulating VTEX would want to report a warning).
);

} // namespace DistanceMapping
