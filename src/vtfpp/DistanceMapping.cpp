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

enum Leaf : uint8_t {
	IN,          // leaf is opaque
	OUT,         // leaf is transparent
	FIGUREITOUT, // leaf is mixed but small so you should brute force scan.
};

uint16_t i32tou16sat(int32_t i, int32_t cap = UINT16_MAX) {
	return static_cast<uint16_t>(std::clamp(i, 0, cap));
}

uint16_t ftoabsu16(float f) {
	return i32tou16sat(static_cast<int32_t>(fabs(f)));
}

using IndexFunction = std::function<int32_t(Vec2i, uint16_t, uint16_t, uint8_t, uint8_t)>;

int32_t indexClamped(Vec2i v, uint16_t w, uint16_t h, uint8_t pxLen, uint8_t alphaOffs) {
	int32_t rX = i32tou16sat(v[0], w - 1);
	int32_t rY = i32tou16sat(v[1], h - 1);
	return (rY * w + rX) * pxLen + alphaOffs;
}

int32_t indexReflected(Vec2i v, uint16_t w, uint16_t h, uint8_t pxLen, uint8_t alphaOffs) {
	int32_t rX = v[0] >= w ? w - v[0] % w : v[0];
	int32_t rY = v[1] >= h ? h - v[1] % h : v[1];
	return (rY * w + rX) * pxLen + alphaOffs;
}

int32_t indexWrapped(Vec2i v, uint16_t w, uint16_t h, uint8_t pxLen, uint8_t alphaOffs) {
	int32_t rX = (v[0] + w) % w;
	int32_t rY = (v[1] + h) % h;
	return (rY * w + rX) * pxLen + alphaOffs;
}

IndexFunction indexBy(ImageConversion::ResizeEdge edge)
{
	switch (edge) {
	default:
		break;
	case ImageConversion::ResizeEdge::REFLECT:
		return indexReflected;
	case ImageConversion::ResizeEdge::WRAP:
		return indexWrapped;
	}
	return indexClamped;
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
		// the second power of two after search radius
		, granularity(uint16_t{4} << ftoabsu16(ceil(log2(static_cast<float>(this->searchRadius)))))
		, offsX(sampleCentered ? reduceX / 2 : 0)
		, offsY(sampleCentered ? reduceY / 2 : 0)
		, alphaThreshold(alphaThreshold)
		, indexFn(indexBy(edge))
	{}

	float sample(Vec2i v) const {
		return this->srcImg[this->indexFn(v, this->imgWidth, this->imgHeight, this->pxLen, this->alphaOffs)];
	}

	Leaf thresh(float alpha) const {
		return alpha >= this->alphaThreshold ? IN : OUT;
	}

	const float *const srcImg;
	const float alphaThreshold;
	const uint16_t imgHeight;
	const uint16_t imgWidth;
	const uint16_t searchRadius;
	const uint16_t granularity;
	const uint16_t offsX, offsY;
	const uint16_t reduceX, reduceY;
	const uint8_t pxLen;
	const uint8_t alphaOffs;
	const IndexFunction indexFn;
};

// immutable map detailing where we can skip computation
struct QuadTree {
	QuadTree(const SampleParam *param, uint32_t &totalComputation) : root(this->scanImg(0, 0, param->imgWidth, param->imgHeight, param, totalComputation))
	{}
	~QuadTree() {
		Node::wither(root);
	}

	struct Node;
	using Branch = std::variant<Leaf, Node *>;
	struct Node {
		const std::array<const Branch, 4> branches;
		Node(Branch nw, Branch ne, Branch sw, Branch se) : branches {nw, ne, sw, se} {};
		~Node() {
			for ( Branch branch : this->branches ) {
				wither(branch);
			}
		}
		static void wither(Branch c) {
			if (holds_alternative<Node *>(c)) {
				delete get<Node*>(c);
			}
		}
	};

	static bool brancheq(Branch a, Branch b) {
		return holds_alternative<Leaf>(a) && holds_alternative<Leaf>(b) && get<Leaf>(a) == get<Leaf>(b);
	}

	const Branch root;
private:
	// TODO: cleverer scan to partition top-down?
	static Branch scanImg(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const SampleParam *param, uint32_t &totalComputation) {
		if (w <= param->granularity || h <= param->granularity) {
			// extend sensitivity to minimum safe distance. a safe leaf is a safe leaf, no need to peek.
			int32_t ix = x - param->searchRadius + param->offsX, tx = x + w + param->searchRadius + param->offsX;
			int32_t iy = y - param->searchRadius + param->offsY, ty = y + h + param->searchRadius + param->offsY;
			Leaf curShade = param->thresh(param->sample({ix, iy}));

			for (auto jx = ix; jx < tx; jx++) {
				for (auto jy = iy; jy < ty; jy++) {
					if (param->thresh(param->sample({jx, jy})) != curShade) {
						totalComputation += w / param->reduceX * h / param->reduceY;
						return FIGUREITOUT;
					}
				}
			}

			return curShade;
		}

		uint16_t hw = w / 2, hh = h / 2;
		Branch iNW = scanImg(x,      y,      hw, hh, param, totalComputation);
		Branch iNE = scanImg(x + hw, y,      hw, hh, param, totalComputation);
		Branch iSW = scanImg(x,      y + hh, hw, hh, param, totalComputation);
		Branch iSE = scanImg(x + hw, y + hh, hw, hh, param, totalComputation);

		if (brancheq(iNW, iNE) && brancheq(iNE, iSW) && brancheq(iSW, iSE)) {
			return get<Leaf>(iNW);
		}
		return new Node(iNW, iNE, iSW, iSE);
	}
};

// traverse a QuadTree at one row of a leaf at a time
class RasterCursor {
public:
	RasterCursor(const QuadTree *tree, uint16_t imgWidth, uint16_t imgHeight, uint16_t reduceX = 1, uint16_t reduceY = 1, uint16_t offsX = 0, uint16_t offsY = 0)
		: tree(tree)
		, imgWidth(imgWidth)
		, imgHeight(imgHeight)
		, reduceX(reduceX)
		, reduceY(reduceY)
		, offsX(offsX)
		, curX(offsX)
		, curY(offsY)
	{
		this->curShade = this->reorient();
	}
	RasterCursor(const QuadTree *tree, const SampleParam *param)
		: RasterCursor(tree, param->imgWidth, param->imgHeight, param->reduceX, param->reduceY, param->offsX, param->offsY)
	{}

	bool getContiguousRun(Leaf &srcShade, uint16_t &srcY, uint16_t &srcX, uint16_t &dstW) {
		srcX = this->curX;
		srcY = this->curY;
		dstW = 0;
		srcShade = this->curShade = this->reorient();

		while (this->curX < this->imgWidth && QuadTree::brancheq(srcShade, this->curShade)) {
			auto blkSize = this->imgWidth >> this->depth;
			this->curX += blkSize;
			dstW += blkSize / this->reduceX;
			this->curShade = this->reorient();
		}

		if (this->curX >= this->imgWidth) {
			this->curX = this->offsX;
			this->curY += this->reduceY;
		}

		return this->curY < this->imgHeight;
	}
private:
	Leaf reorient() {
		this->depth = 0;
		this->curBranch = this->tree->root;
		auto remX = this->curX, remY = this->curY;

		while (std::holds_alternative<QuadTree::Node *>(this->curBranch)) {
			auto benchX = this->imgWidth >> (this->depth + 1), benchY = this->imgHeight >> (this->depth + 1);
			if (benchX < 2 || benchY < 2) {
				return this->curShade; // would be nice to make this unrepresentable
			}
			auto bGtX = remX >= benchX, bGtY = remY >= benchY;
			remX -= benchX * bGtX;
			remY -= benchY * bGtY;
			this->curBranch = std::get<QuadTree::Node *>(this->curBranch)->branches[bGtX | bGtY << 1];
			this->depth++;
		}

		return get<Leaf>(this->curBranch);
	}

	const QuadTree *tree;
	uint16_t curX, curY, depth;
	const uint16_t imgWidth, imgHeight, reduceX, reduceY, offsX;
	Leaf curShade;
	QuadTree::Branch curBranch;
};

float vecDist(Vec2f32 v0, Vec2f32 v1) {
	return std::hypot(v0[0] - v1[0], v1[0] - v1[1]);
}
Vec2i vec2ipmul(Vec2i v0, Vec2i v1) {
	return {v0[0] * v1[0], v0[1] * v1[1]};
}

using math::pi_f32;

void paintMap(
	const SampleParam *param,
	float *dstImg,
	uint8_t dstPxLen,
	uint8_t dstAlphaOffs,
	bool distanceAA,
	bool euclidean,
	const float *tangentDiffusion,
	bool *valveQuirks
) {
	uint32_t totalComputation = 0;

	const QuadTree tree(param, totalComputation);
	RasterCursor cursor(&tree, param);

	uint32_t iOut = dstAlphaOffs;
	Leaf srcShade = FIGUREITOUT;
	uint16_t srcY = 0, srcX = 0, dstW = 0;

	float fRadius = static_cast<float>(param->searchRadius);
	uint16_t dstWidth = param->imgWidth / param->reduceX;
	uint16_t dstHeight = param->imgHeight / param->reduceY;

	uint32_t iGradW = 0;
	std::vector<float> gradients = tangentDiffusion ? std::vector<float>(totalComputation, -pi_f32 - 0.05f) : std::vector<float>(0);
	float lastAng = 0;
	float hypot1 = 1;

	bool keepPainting = true;
	do {
		keepPainting = cursor.getContiguousRun(srcShade, srcY, srcX, dstW);
		float fillDistance = 1.0f;
		switch (srcShade) {
		case OUT:
			fillDistance = 0.0f;
		case IN:
			for (uint16_t i = 0; i < dstW; i++, iOut += dstPxLen) {
				dstImg[iOut] = fillDistance;
			}
			break;
		case FIGUREITOUT:
		default:
			// oh no we actually have to do work
			for (uint16_t i = 0; i < dstW; i++, iGradW++, iOut += dstPxLen) {
				int32_t curX = srcX + i * param->reduceX;
				float alphaRef = param->sample({curX, srcY});
				Leaf stateRef = param->thresh(alphaRef);
				float nearest = std::numeric_limits<float>::max();
				for (int32_t sX = curX - param->searchRadius, tX = curX + param->searchRadius; sX < tX; sX++) {
					for (int32_t sY = srcY - param->searchRadius, tY = srcY + param->searchRadius; sY < tY; sY++) {
						float alphaSmp = param->sample({sX, sY});
						Leaf stateSmp = param->thresh(alphaSmp);
						if (stateSmp != stateRef) {
							float dist = std::hypot<float>(sX - curX, sY - srcY);

							if (tangentDiffusion || distanceAA || euclidean) {
								lastAng = atan2(sX - curX, sY - srcY);
							}
							if (distanceAA || euclidean) {
								hypot1 = 1 / cos(fabs(fmod(lastAng + pi_f32 * 2.25f, pi_f32 / 2.0f) - pi_f32 / 4.0f));
							}
							if (distanceAA) {
								dist += hypot1 * (1 - fabs(alphaSmp - alphaRef));
							}
							if (euclidean && dist > fRadius + hypot1) {
								continue;
							}
							nearest = fmin(nearest, dist);
						}
					}
				}

				nearest = fmin(0.5f, nearest * 0.5f / fRadius);
				if (stateRef == OUT) {
					nearest = -nearest;
				}

				dstImg[iOut] = 0.5 + nearest;
				if (tangentDiffusion) {
					gradients[iGradW] = lastAng;
				}
			}
		}
	} while (keepPainting);

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
		// is that we can continue ignoring pixels we previously ignored thanks
		// to the quadtree (and thus, never set any angle for): we're diffusing
		// error tangent to the gradient, and that way lie only pixels where we
		// also already set an angle. we can index angles independently, which,
		// depending on the contents of the source image, might require a small
		// fraction of the space compared to the distance map (or just as much)
		static constexpr std::array<const int8_t, 8> hardSigns{0, 1, 1, 1, 0, -1, -1, -1};
		// of course, in our case, y grows downward, so we really want is E..NW
		// which, conveniently enough, is just 0..3, so no aux lookup table. if
		// we wanted SW..E, we'd index into {0, 5, 6, 7}. the pixel offsets are
		// the only result where we need to be picky about the final signs; the
		// preceding calculations can run in our flipped coordinates just fine.
		static constexpr float eighth = pi_f32 / 4.0f;

		RasterCursor diffCursor(&tree, dstWidth, dstHeight);
		uint32_t diffOut = dstAlphaOffs;
		Leaf diffShade = FIGUREITOUT;
		const uint32_t dstBufLen = dstWidth * dstHeight * dstPxLen;
		uint16_t diffY = 0, diffX = 0, diffW = 0;
		uint32_t iGradR = 0;

		bool keepDiffusing = true;
		do {
			keepDiffusing = diffCursor.getContiguousRun(diffShade, diffY, diffX, diffW);
			if (diffShade != FIGUREITOUT) {
				continue;
			}
			bool rightEdge = diffX + diffW >= dstWidth;

			for (uint16_t i = 0; i < diffW; i++, iGradR++, diffOut += dstPxLen) {
				float theta = gradients[iGradR];
				if (theta < -pi_f32 - 0.025f) {
					continue;
				}
				float thetaNormal = fmod(theta + 2.5f * pi_f32, 2.0f * pi_f32);
				uint8_t nEighths = static_cast<uint32_t>(std::floor(thetaNormal / eighth)) % 8;
				double quantum = *tangentDiffusion;
				double curPx = dstImg[diffOut];
				double quantized = std::round(curPx / quantum) * quantum;
				float quantErr = curPx - quantized;
				dstImg[diffOut] = quantized;
				bool atRight = rightEdge && i >= diffW - 1;
				bool bottomEdge = diffY >= dstHeight - 1;
				if (atRight && bottomEdge) {
					break;
				}
				Vec2f32 normal{cos(thetaNormal), sin(thetaNormal)};
				// distance calculation uses the whole circle
				float dist0 = vecDist(normal, {hardSigns[(nEighths + 2) % 8], hardSigns[(nEighths + 0) % 8]});
				float dist1 = vecDist(normal, {hardSigns[(nEighths + 3) % 8], hardSigns[(nEighths + 1) % 8]});
				// offset selection uses the part of the circle that is yet to be scanned
				Vec2i offs0{hardSigns[nEighths % 4 + 2], hardSigns[nEighths % 4 + 0]};
				Vec2i offs1{hardSigns[nEighths % 4 + 3], hardSigns[nEighths % 4 + 1]};
				Vec2i pos{diffX + i, diffY};
				if (bottomEdge) {
					offs0 = vec2ipmul(offs0, {1, 0});
					offs1 = vec2ipmul(offs1, {1, 0});
				}
				dstImg[param->indexFn(pos + offs0, dstWidth, dstHeight, dstPxLen, dstAlphaOffs)] += quantErr * dist1 / (dist0 + dist1);
				dstImg[param->indexFn(pos + offs1, dstWidth, dstHeight, dstPxLen, dstAlphaOffs)] += quantErr * dist0 / (dist0 + dist1);
			}
		} while (keepDiffusing);
	}

	if (valveQuirks) {
		*valveQuirks = false;

		auto blackOutAndWarn = [&](auto oi) {
			float& px = dstImg[oi * dstPxLen + dstAlphaOffs];
			if (fabs(px) > 0.000001f) {
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

	float quantum = 1.0f / static_cast<float>(1 << decompressedAlpha(outFormat));
	bool doGradientDither = dither == Dither::GRADIENT_TANGENT && !decimal(outFormat);
	paintMap(
		&param,
		reinterpret_cast<float *>(dstImg.data()),
		dstPxLen,
		dstAlphaOffs,
		!!(flags & Flags::DISTANCEAA),
		!!(flags & Flags::EUCLIDEAN),
		doGradientDither ? &quantum : nullptr,
		valveQuirks
	);

	return (outFormat == intermediateWrite) ? dstImg : convertImageDataToFormat(dstImg, intermediateWrite, outFormat, dstWidth, dstHeight);
}
