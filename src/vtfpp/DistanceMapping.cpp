#include <vtfpp/DistanceMapping.h>

#include <variant>

#include <sourcepp/Macros.h>
#include <sourcepp/MathExtended.h>
#include <vtfpp/ImageConversion.h>

using namespace sourcepp;
using namespace vtfpp;

namespace vtfpp::DistanceMapping::detail {
namespace {

using namespace ImageConversion;
using math::Vec2i;
using math::Vec2f32;

uint16_t ftoabsu16(float f) {
	return static_cast<uint16_t>(std::clamp<uint16_t>(static_cast<int32_t>(fabs(f)), 0, UINT16_MAX));
}

// literally just stb. grep '_full( int n' in stb_image_resize2.h
int32_t edgeClamp(int32_t n, int32_t max) {
	if (n < 0) {
		return 0;
	}
	return (n >= max) ? max - 1 : n;
}
int32_t edgeReflect(int32_t n, int32_t max) {
	if (n < 0) {
		return n > -max ? -n : max - 1;
	}
	if (n >= max) {
		const int32_t max2 = 2 * max;
		return n >= max2 ? 0 : max2 - n - 1;
	}
	return n;
}
int32_t edgeWrap(int32_t n, int32_t max) {
	if (n >= 0) {
		return n % max;
	}
	const int32_t m = (-n) % max;
	return m ? max - m : 0;
}

int32_t indexWith(ImageConversion::ResizeEdge edge, Vec2i v, uint16_t w, uint16_t h, uint8_t pxLen, uint8_t alphaOffs )
{
	switch (edge) {
	default:
		break;
	case ImageConversion::ResizeEdge::REFLECT:
		return (edgeReflect(v[1], h) * w + edgeReflect(v[0], w)) * pxLen + alphaOffs;
	case ImageConversion::ResizeEdge::WRAP:
		return (edgeWrap(v[1], h) * w + edgeWrap(v[0], w)) * pxLen + alphaOffs;
	}
	return (edgeClamp(v[1], h) * w + edgeClamp(v[0], w)) * pxLen + alphaOffs;
}

// big ugly immutable record of everything pertaining to how we read alpha from an image
class SampleParam {
public:
	SampleParam(
		const float *srcImg,
		uint16_t inWidth,
		uint16_t inHeight,
		uint16_t reduceX,
		uint16_t reduceY,
		uint8_t pxLen,
		uint8_t alphaOffs,
		float distanceSpread,
		bool sampleCentered,
		float alphaThreshold,
		ImageConversion::ResizeEdge edge
	)
		: srcImg(srcImg)
		, pxLen(pxLen)
		, alphaOffs(alphaOffs)
		, imgWidth(inWidth)
		, imgHeight(inHeight)
		, reduceX(reduceX)
		, reduceY(reduceY)
		, searchRadius(ftoabsu16(2.0f * std::max<float>(reduceX, reduceY) * distanceSpread))
		, offsX(sampleCentered ? reduceX / 2 : 0)
		, offsY(sampleCentered ? reduceY / 2 : 0)
		, alphaThreshold(alphaThreshold)
		, edge(edge)
	{}

	// n.b.: "padded" by search radius so the distance transform can read past
	// the edges in accordance with the edge policy
	float samplePadded(Vec2i v) const {
		return this->srcImg[indexWith(this->edge, v - this->searchRadius, this->imgWidth, this->imgHeight, this->pxLen, this->alphaOffs)];
	}

	bool isInPadded(Vec2i v) const {
		return this->samplePadded(v) >= this->alphaThreshold;
	}

	const float *const srcImg;
	const float alphaThreshold;
	const uint16_t imgHeight;
	const uint16_t imgWidth;
	const uint16_t reduceX, reduceY;
	const uint16_t searchRadius;
	const uint16_t offsX, offsY;
	const uint8_t pxLen;
	const uint8_t alphaOffs;
	const ImageConversion::ResizeEdge edge;
};

float vecDist(Vec2f32 v0, Vec2f32 v1) {
	return std::hypot(v0[0] - v1[0], v0[1] - v1[1]);
}

using math::pi_f32;

static constexpr float BIG_EPSILON = 0.05f;
static constexpr float SMALL_EPSILON = 1e-6f;

// scuffed at sharp angled points but uh. horseshoes, government work, etc
float aaOffsetPadded(const SampleParam *param, Vec2i v, bool outside) {
	const float alpha = param->samplePadded(v);
	const float reach = outside ? 1.0f : 0.5f;
	float slope = 0.0f;
	for (Vec2i offset : {Vec2i{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}}) {
		slope = fmax(slope, fabs(param->samplePadded(v + offset) - alpha) / std::hypot<float>(offset[0], offset[1]));
	}
	const float gap = fabs(alpha - 0.5f);
	if (slope < SMALL_EPSILON || gap > reach * slope) {
		return outside ? 0.0f : -1.0f;
	}
	return fmin(gap / slope, reach);
}

template<typename T>
T sq(T f) {
	return f * f;
}

static constexpr float inff = std::numeric_limits<float>::infinity();
static constexpr double infd = std::numeric_limits<double>::infinity();

// Distance Transforms of Sampled Functions
// Pedro F. Felzenszwalb, Daniel P. Huttenlocher.
// https://cs.brown.edu/people/pfelzens/papers/dt-final.pdf
//
// mostly just the 1D transform on page 6 (420) but:
// - optionally report argmin of the envelope bc aa/dither need pixel positions
//   - this still isn't perfect information on its own, but *is* enough to
//     determine whether whipping out the ol raster scan is necessary
// - cut off outside a particular window
void distanceTransform1D(
	const float *f,
	int32_t n,
	float *d,
	int32_t *argmin,
	int32_t window,
	int32_t *v,
	double *z,
	int32_t first,
	int32_t stride
) {
	int32_t q0 = 0;
	while (q0 < n && std::isinf(f[q0])) {
		q0++;
	}
	if (q0 == n) {
		const int32_t m = (n - first + stride - 1) / stride;
		std::fill(d, d + m, inff);
		if (argmin) {
			std::fill(argmin, argmin + m, -1);
		}
		return;
	}

	int32_t k = 0;
	v[0] = q0;
	z[0] = -infd;
	z[1] = +infd;
	for (int32_t q = q0 + 1; q < n; q++) {
		if (std::isinf(f[q])) {
			continue;
		}
	six: // i love academic pseudo algol very cool
		const double s = ((f[q] + sq<double>(q)) - (f[v[k]] + sq<double>(v[k]))) / (2.0 * (q - v[k]));
		if (s <= z[k]) {
			k--;
			goto six;
		}
		k++;
		v[k] = q;
		z[k] = s;
		z[k + 1] = infd;
	}

	k = 0;
	for (int32_t q = first, j = 0; q < n; q += stride, j++) {
		while (z[k + 1] < q) {
			k++;
		}
		const bool out = v[k] < q - window || v[k] > q + window - 1;
		d[j] = out ? inff : f[v[k]] + sq<float>(q - v[k]);
		if (argmin) {
			argmin[j] = out ? -1 : v[k];
		}
	}
}

void paintMap(
	const SampleParam *param,
	float *dstImg,
	uint8_t dstPxLen,
	uint8_t dstAlphaOffs,
	bool distanceAA,
	const float *tangentDiffusion,
	bool *valveQuirks
) {
	const auto [padW, padH] = (math::Vec2ui{param->imgWidth, param->imgHeight} + 2 * param->searchRadius).values;
	std::vector<uint8_t> mask(padW * padH); // vector<bool> was slower dont @ me
	for (uint32_t padY = 0; padY < padH; padY++) {
		for (uint32_t padX = 0; padX < padW; padX++) {
			mask[padX * padH + padY] = param->isInPadded(Vec2i{padX, padY});
		}
	}

	float fRadius = static_cast<float>(param->searchRadius);
	uint16_t dstWidth = param->imgWidth / param->reduceX;
	uint16_t dstHeight = param->imgHeight / param->reduceY;

	std::vector<float> gradients(tangentDiffusion ? static_cast<size_t>(dstWidth) * dstHeight : 0, -pi_f32 - BIG_EPSILON);

	const bool needArgmin = tangentDiffusion || distanceAA;
	const size_t padMax = std::max(padW, padH);
	std::vector<float> df2D(padW * dstHeight);
	std::vector<float> df1D(padMax);
	std::vector<int32_t> argmin2D(needArgmin ? padW * dstHeight : 0);
	std::vector<int32_t> argmin1D(padMax);
	std::vector<float> col(padH);
	std::vector<int32_t> seedTally(needArgmin ? padW + 1 : 0);
	std::vector<int32_t> v(padMax);
	std::vector<double> z(padMax + 1);

	for (uint8_t seed : {1, 0}) {
		for (uint32_t padX = 0; padX < padW; padX++) {
			for (uint32_t padY = 0; padY < padH; padY++) {
				col[padY] = mask[padX * padH + padY] == seed ? 0.0f : inff;
			}
			distanceTransform1D(
				col.data(), padH, df1D.data(),
				needArgmin ? argmin1D.data() : nullptr,
				param->searchRadius,
				v.data(), z.data(),
				param->offsY + param->searchRadius, param->reduceY
			);
			for (uint32_t dstY = 0; dstY < dstHeight; dstY++) {
				df2D[dstY * padW + padX] = df1D[dstY];
				if (needArgmin) {
					argmin2D[dstY * padW + padX] = argmin1D[dstY];
				}
			}
		}

		for (uint32_t dstY = 0; dstY < dstHeight; dstY++) {
			const int32_t padY = dstY * param->reduceY + param->offsY + param->searchRadius;
			distanceTransform1D(
				df2D.data() + dstY * padW, padW, df1D.data(),
				needArgmin ? argmin1D.data() : nullptr,
				padW,
				v.data(), z.data(),
				param->offsX + param->searchRadius, param->reduceX
			);

			// running count for quick ez checks below; cf. seedInWindow
			if (needArgmin) {
				seedTally[0] = 0;
				for (uint32_t c = 0; c < padW; c++) {
					seedTally[c + 1] = seedTally[c] + !std::isinf(df2D[dstY * padW + c]);
				}
			}
			for (uint32_t dstX = 0; dstX < dstWidth; dstX++) {
				const int32_t padX = dstX * param->reduceX + param->offsX + param->searchRadius;
				if (mask[padX * padH + padY] == seed) {
					continue;
				}
				const int32_t windowStart = padX - param->searchRadius;
				const int32_t windowEnd = padX + param->searchRadius;
				const bool seedInWindow = needArgmin && seedTally[windowEnd] > seedTally[windowStart];
				float nearest = sqrtf(df1D[dstX]);
				int32_t nearestX = seedInWindow ? argmin1D[dstX] : -1;
				if (seedInWindow && nearest >= fRadius) {
					float nearest2 = std::numeric_limits<float>::max();
					int32_t windowCol = -1;
					for (int32_t c = windowStart; c < windowEnd; c++) {
						if (const float dist2 = df2D[dstY * padW + c] + sq<float>(padX - c); dist2 < nearest2) {
							nearest2 = dist2;
							windowCol = c;
						}
					}
					if (windowCol >= 0) {
						nearest = sqrtf(nearest2);
						nearestX = windowCol;
					}
				}
				int32_t nearestY = -1;
				if (distanceAA) {
					if (seedInWindow) {
						const float outer = nearest + 1;
						const float outer2 = sq(outer);
						const float inner2 = sq(nearest) - 1;
						const int32_t loX = -std::min<int32_t>(outer,     param->searchRadius);
						const int32_t hiX = +std::min<int32_t>(outer + 1, param->searchRadius);
						const float r2 = sq(param->searchRadius);
						float preciseNearest = inff;

						for (int32_t dX = loX; dX < hiX; dX++) {
							const float dX2 = sq<float>(dX);
							const float remHi = outer2 - dX2;
							if (remHi < 0.0f || dX2 + r2 < inner2) {
								continue;
							}
							const int32_t hi = sqrtf(remHi);
							const int32_t lo = inner2 > dX2 ? ceilf(sqrtf(inner2 - dX2)) : 0;
							const int32_t aaX = padX + dX;
							const int32_t loY = -std::min<int32_t>(hi,     param->searchRadius);
							const int32_t hiY = +std::min<int32_t>(hi + 1, param->searchRadius);
							for (int32_t dY = loY; dY < hiY; dY++) {
								if (-lo < dY && dY < lo) {
									dY = lo - 1;
									continue;
								}
								const int32_t aaY = padY + dY;
								if (mask[aaX * padH + aaY] != seed) {
									continue;
								}
								// it would maybe make more "sense" to just say
								// aa means you should set threshold 0.5, but
								// threshold is useful to keep independent,
								// because image editors are stupid and love
								// discarding the rgb values of transparent
								// pixels. even tgas. and as long as that stays
								// the case, we want to enable the trick of
								// exporting with a background at 1/255 alpha
								// to force the image to retain rgb there.
								// btw if you didn't know that trick and this
								// is how you find out, you're welcome :^)
								float offsSeed = aaOffsetPadded(param, {aaX, aaY}, true);
								if (seed && param->samplePadded({aaX, aaY}) < 0.5f) {
									offsSeed = -offsSeed;
								}
								const float preciseDist = static_cast<float>(std::hypot(dX, dY)) - offsSeed;
								if (preciseDist < preciseNearest) {
									preciseNearest = preciseDist;
									nearestX = aaX;
									nearestY = aaY;
								}
							}
						}
						if (preciseNearest < inff) {
							nearest = preciseNearest;
						}
					}
					const float offsRef = aaOffsetPadded(param, {padX, padY}, false);
					if (offsRef >= 0.0f) {
						nearest = fmin(nearest, offsRef);
					}
				}

				if (nearestX >= 0 && nearestY < 0) {
					nearestY = argmin2D[dstY * padW + nearestX];
				}
				nearest = fmax(nearest, 0.0f);
				nearest = fmin(0.5f, nearest * 0.5f / fRadius);
				if (seed) {
					nearest = -nearest;
				}

				const uint32_t iDst = dstY * dstWidth + dstX;
				dstImg[iDst * dstPxLen + dstAlphaOffs] = 0.5 + nearest;
				if (tangentDiffusion) {
					gradients[iDst] = nearestX >= 0 ? atan2(nearestY - padY, nearestX - padX) : (-pi_f32 - BIG_EPSILON);
				}
			}
		}
	}

	if (tangentDiffusion) {
		// attempt to diffuse quantization error tangent to the gradient of the
		// distance map. you could imagine some approach that involves grouping
		// greyscale pixels into distinct contours, and playing ring around the
		// rosy for each group, but there's no practical way to avoid littering
		// residual bits of error into pixels you've already quantized, ruining
		// the whole point. the usual suspects like floyd-steinberg use kernels
		// shaped to only write *ahead* of a conventional raster scan. here, we
		// do the same by determining a shape, bounded by the following "mask":
		//
		// ⎡ 0 0 0 ⎤
		// ⎢ 0 0 1 ⎥
		// ⎣ 1 1 1 ⎦,
		//
		// mapping adjacent pixels to the unit circle's bounding box like this:
		//
		// ⎡ NW:(-1,  1)  N:(0,  1)  NE:(1,  1) ⎤
		// ⎢  W:(-1,  0)              E:(1,  0) ⎥
		// ⎣ SW:(-1, -1)  S:(0, -1)  SE:(1, -1) ⎦,
		//
		// choose the two points nearest (x, y) on the unit circle at θ + π / 2
		// (i.e. the right-hand normal of the gradient direction θ). distribute
		// the quantization error E between the two, weighted by the opposite's
		// distance over the sum of both distances (it feels as though a better
		// weighting should exist but that's what i get for dropping out), then
		// fold the whole thing over to only entries ahead of us in scan order:
		//
		//     ⎡                   ⎤
		// k = ⎢              E+W  ⎥
		//     ⎣ SW+NE  S+N  SE+NW ⎦,
		//
		// yielding our per-destination-pixel kernel k. a neat property of this
		// is that we can ignore pixels that aren't within radius of one of the
		// parabolas (and thus, never set any angle for): we're diffusing error
		// tangent to the gradient, and that way lie only pixels where we would
		// also find a parabola. we can't index angles *independently* anymore,
		// but oh well who cares. at least we can still commit to this terrible
		// bit of torturing every single line of this explanation to 68 columns
		static constexpr std::array<const int8_t, 8> hardSigns{0, 1, 1, 1, 0, -1, -1, -1};
		// of course, in our case, y grows downward, so we really want is E..NW
		// which, conveniently enough, is just 0..3, so no aux lookup table. if
		// we wanted SW..E, we'd index into {0, 5, 6, 7}. the pixel offsets are
		// the only result where we need to be picky about the final signs; the
		// preceding calculations can run in our flipped coordinates just fine.
		static constexpr float eighth = pi_f32 / 4.0f;

		const double quantum = *tangentDiffusion;
		for (uint32_t diffY = 0; diffY < dstHeight; diffY++) {
			for (uint32_t diffX = 0; diffX < dstWidth; diffX++) {
				const float theta = gradients[diffY * dstWidth + diffX];
				if (theta < -pi_f32 - BIG_EPSILON / 2.0f) {
					continue;
				}
				float thetaNormal = fmod(theta + 2.5f * pi_f32, 2.0f * pi_f32);
				uint8_t nEighths = static_cast<uint32_t>(std::floor(thetaNormal / eighth)) % 8;
				Vec2i pos{diffX, diffY};
				uint32_t iDiff = indexWith(param->edge, pos, dstWidth, dstHeight, dstPxLen, dstAlphaOffs);
				double curPx = dstImg[iDiff];

				double quantized = std::round(curPx / quantum) * quantum;
				float quantErr = curPx - quantized;
				dstImg[iDiff] = quantized;
				bool atRight = diffX + 1 >= dstWidth;
				bool bottomEdge = pos[1] >= dstHeight - 1;

				if (atRight && bottomEdge) {
					break;
				}
				Vec2f32 normal{cos(thetaNormal), sin(thetaNormal)};
				// distance calculation uses the whole circle
				float dist0 = vecDist(normal, {hardSigns[(nEighths + 2) % 8], hardSigns[(nEighths + 0) % 8]});
				float dist1 = vecDist(normal, {hardSigns[(nEighths + 3) % 8], hardSigns[(nEighths + 1) % 8]});
				// offset selection uses the part of the circle that is yet to be scanned
				Vec2i offs0{hardSigns[(nEighths + 0) % 4 + 2], bottomEdge ? 0 : hardSigns[(nEighths + 0) % 4]};
				Vec2i offs1{hardSigns[(nEighths + 1) % 4 + 2], bottomEdge ? 0 : hardSigns[(nEighths + 1) % 4]};

				dstImg[indexWith(param->edge, pos + offs0, dstWidth, dstHeight, dstPxLen, dstAlphaOffs)] += quantErr * dist1 / (dist0 + dist1);
				dstImg[indexWith(param->edge, pos + offs1, dstWidth, dstHeight, dstPxLen, dstAlphaOffs)] += quantErr * dist0 / (dist0 + dist1);
			}
		}
	}

	if (valveQuirks) {
		*valveQuirks = false;

		auto blackOutAndWarn = [&](auto oi) {
			float& px = dstImg[oi * dstPxLen + dstAlphaOffs];
			if (fabs(px) > SMALL_EPSILON) {
				*valveQuirks = true;
			}
			px = 0.0f;
		};

		for (auto x = 0; x < dstWidth; x++) {
			blackOutAndWarn(x);
			blackOutAndWarn(x + (dstHeight - 1) * dstWidth);
		}
		for (auto y = 0; y < dstHeight; y++) {
			blackOutAndWarn(y * dstWidth);
			blackOutAndWarn(y * dstWidth + dstWidth - 1);
		}
	}
}

bool operator!(Flags flags) {
	return flags == Flags::NONE;
};

bool isSingleChannel(ImageFormat format) {
	using namespace ImageFormatDetails;
	return red(format) == bpp(format) && bpp(format);
}

} // namespace
} // namespace vtfpp::DistanceMapping::detail

[[nodiscard]] std::vector<std::byte> DistanceMapping::alphaToDistance(std::span<const std::byte> imageData, ImageFormat inFormat, ImageFormat outFormat, uint16_t width, uint16_t height, uint16_t reduceX, uint16_t reduceY, bool srgb, float distanceSpread, float alphaThreshold, DistanceMapping::Flags flags, DistanceMapping::Dither dither, ImageConversion::ResizeFilter filter, ImageConversion::ResizeEdge edge, bool *valveQuirks) {
	using namespace vtfpp::DistanceMapping::detail;
	using namespace ImageFormatDetails;

	auto mkFormat = [](ImageFormat format) {
		return isSingleChannel(format) ?
			std::make_tuple(ImageFormat::R32F, uint8_t{1}, uint8_t{0})
		: (decompressedAlpha(format) || alpha(format)) ?
			std::make_tuple(ImageFormat::RGBA32323232F, uint8_t{4}, uint8_t{3})
		:
			std::make_tuple(ImageFormat::EMPTY, uint8_t{0}, uint8_t{0});
	};
	auto [intermediateRead, srcPxLen, srcAlphaOffs] = mkFormat(inFormat);
	auto [intermediateWrite, dstPxLen, dstAlphaOffs] = mkFormat(outFormat);

	if (
		   intermediateRead == ImageFormat::EMPTY
		|| intermediateWrite == ImageFormat::EMPTY
		|| !math::isPowerOf2(reduceX)
		|| !math::isPowerOf2(reduceY)
		|| ftoabsu16(distanceSpread * reduceX) < 1
		|| ftoabsu16(distanceSpread * reduceY) < 1
		|| std::clamp(alphaThreshold, 0.0f, 1.0f) != alphaThreshold
		|| edge == ImageConversion::ResizeEdge::ZERO
	) {
		return {};
	}

	bool inputNeedsConversion = intermediateRead != inFormat;
	std::vector<std::byte> i_love_raii = inputNeedsConversion ? convertImageDataToFormat(imageData, inFormat, intermediateRead, width, height) : std::vector<std::byte>(0);
	std::span<const std::byte> srcImg = inputNeedsConversion ? i_love_raii : imageData;

	SampleParam param(reinterpret_cast<const float *>(srcImg.data()), width, height, reduceX, reduceY, srcPxLen, srcAlphaOffs, distanceSpread, !!(flags & Flags::SAMPLECENTERED), alphaThreshold, edge);

	uint16_t dstWidth = width / reduceX, dstHeight = height / reduceY;

	std::vector<std::byte> dstImg
		= (!isSingleChannel(inFormat) && !isSingleChannel(outFormat)) ?
			convertImageDataToFormat(
				resizeImageData(imageData, inFormat, width, dstWidth, height, dstHeight, srgb, true, filter, edge),
				inFormat, intermediateWrite, dstWidth, dstHeight
			)
		:
			std::vector<std::byte>(sizeof(float) * dstPxLen * dstWidth * dstHeight, std::byte{0x00});

	int8_t alphaBits = decompressedAlpha(outFormat);
	float quantum = (alphaBits > 0) ? 1.0f / static_cast<float>((1 << alphaBits) - 1) : 1.0f;
	bool doGradientDither = dither == Dither::GRADIENT_TANGENT && !decimal(outFormat);
	paintMap(
		&param,
		reinterpret_cast<float *>(dstImg.data()),
		dstPxLen,
		dstAlphaOffs,
		!!(flags & Flags::DISTANCEAA),
		doGradientDither ? &quantum : nullptr,
		valveQuirks
	);

	return (outFormat == intermediateWrite) ? dstImg : convertImageDataToFormat(dstImg, intermediateWrite, outFormat, dstWidth, dstHeight);
}
