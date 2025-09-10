#include <vtfpp/VTF.h>

#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <utility>

#ifdef SOURCEPP_BUILD_WITH_TBB
#include <execution>
#endif

#ifdef SOURCEPP_BUILD_WITH_THREADS
#include <future>
#include <thread>
#endif

#include <BufferStream.h>
#include <miniz.h>
#include <zstd.h>

#include <sourcepp/compression/LZMA.h>
#include <vtfpp/ImageConversion.h>

using namespace sourcepp;
using namespace vtfpp;

namespace {

[[nodiscard]] std::vector<std::byte> compressData(std::span<const std::byte> data, int16_t level, CompressionMethod method) {
	switch (method) {
		using enum CompressionMethod;
		case DEFLATE: {
			mz_ulong compressedSize = mz_compressBound(data.size());
			std::vector<std::byte> out(compressedSize);

			int status = MZ_OK;
			while ((status = mz_compress2(reinterpret_cast<unsigned char*>(out.data()), &compressedSize, reinterpret_cast<const unsigned char*>(data.data()), data.size(), level)) == MZ_BUF_ERROR) {
				compressedSize *= 2;
				out.resize(compressedSize);
			}

			if (status != MZ_OK) {
				return {};
			}
			out.resize(compressedSize);
			return out;
		}
		case ZSTD: {
			if (level < 0) {
				level = 6;
			}

			const auto expectedSize = ZSTD_compressBound(data.size());
			std::vector<std::byte> out(expectedSize);

			const auto compressedSize = ZSTD_compress(out.data(), expectedSize, data.data(), data.size(), level);
			if (ZSTD_isError(compressedSize)) {
				return {};
			}

			out.resize(compressedSize);
			return out;
		}
		case CONSOLE_LZMA: {
			if (const auto out = compression::compressValveLZMA(data, level)) {
				return *out;
			}
			return {};
		}
	}
	return {};
}

template<bool ExistingDataIsSwizzled>
constexpr void swizzleUncompressedImageData(std::span<std::byte> inputData, std::span<std::byte> outputData, ImageFormat format, uint16_t width, uint16_t height, uint16_t slice) {
	width *= ImageFormatDetails::bpp(format) / 32;
	const auto zIndex = [
		widthL2 = static_cast<int>(math::log2ceil(width)),
		heightL2 = static_cast<int>(math::log2ceil(height)),
		sliceL2 = static_cast<int>(math::log2ceil(slice))
	](uint32_t x, uint32_t y, uint32_t z) {
		auto widthL2m = widthL2;
		auto heightL2m = heightL2;
		auto sliceL2m = sliceL2;
		uint32_t offset = 0;
		uint32_t shiftCount = 0;
		do {
			if (sliceL2m --> 0) {
				offset |= (z & 1) << shiftCount++;
				z >>= 1;
			}
			if (heightL2m --> 0) {
				offset |= (y & 1) << shiftCount++;
				y >>= 1;
			}
			if (widthL2m --> 0) {
				offset |= (x & 1) << shiftCount++;
				x >>= 1;
			}
		} while (x | y | z);
		return offset;
	};

	const auto* inputPtr = reinterpret_cast<const uint32_t*>(inputData.data());
	auto* outputPtr = reinterpret_cast<uint32_t*>(outputData.data());
	for (uint16_t x = 0; x < width; x++) {
		for (uint16_t y = 0; y < height; y++) {
			for (uint16_t z = 0; z < slice; z++) {
				if constexpr (ExistingDataIsSwizzled) {
					*outputPtr++ = reinterpret_cast<const uint32_t*>(inputData.data())[zIndex(x, y, z)];
				} else {
					reinterpret_cast<uint32_t*>(outputData.data())[zIndex(x, y, z)] = *inputPtr++;
				}
			}
		}
	}
}

template<bool ConvertingFromSource>
void swapImageDataEndianForConsole(std::span<std::byte> imageData, ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount, VTF::Platform platform) {
	if (imageData.empty() || format == ImageFormat::EMPTY || platform == VTF::PLATFORM_PC) {
		return;
	}

	if (platform == VTF::PLATFORM_X360) {
		switch (format) {
			using enum ImageFormat;
			case BGRA8888:
			case BGRX8888:
			case UVWQ8888:
			case UVLX8888: {
				const auto newData = ImageConversion::convertSeveralImageDataToFormat(imageData, ARGB8888, BGRA8888, mipCount, frameCount, faceCount, width, height, sliceCount);
				std::ranges::copy(newData, imageData.begin());
				break;
			}
			case DXT1:
			case DXT1_ONE_BIT_ALPHA:
			case DXT3:
			case DXT5:
			case UV88: {
				std::span dxtData{reinterpret_cast<uint16_t*>(imageData.data()), imageData.size() / sizeof(uint16_t)};
				std::for_each(
#ifdef SOURCEPP_BUILD_WITH_TBB
						std::execution::par_unseq,
#endif
						dxtData.begin(), dxtData.end(), [](uint16_t& value) {
					BufferStream::swap_endian(&value);
				});
				break;
			}
			default:
			break;
		}
	}

	if ((platform == VTF::PLATFORM_XBOX || platform == VTF::PLATFORM_PS3_ORANGEBOX || platform == VTF::PLATFORM_PS3_PORTAL2) && !ImageFormatDetails::compressed(format) && ImageFormatDetails::bpp(format) % 32 == 0) {
		std::vector<std::byte> out(imageData.size());
		for(int mip = mipCount - 1; mip >= 0; mip--) {
			const auto mipWidth = ImageDimensions::getMipDim(mip, width);
			const auto mipHeight = ImageDimensions::getMipDim(mip, height);
			for (int frame = 0; frame < frameCount; frame++) {
				for (int face = 0; face < faceCount; face++) {
					if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, format, mip, mipCount, frame, frameCount, face, faceCount, width, height)) {
						std::span imageDataSpan{imageData.data() + offset, length * sliceCount};
						std::span outSpan{out.data() + offset, length * sliceCount};
						::swizzleUncompressedImageData<ConvertingFromSource>(imageDataSpan, outSpan, format, mipWidth, mipHeight, sliceCount);
					}
				}
			}
		}
		std::memcpy(imageData.data(), out.data(), out.size());
	}
}

template<bool ConvertingFromDDS>
[[nodiscard]] std::vector<std::byte> convertBetweenDDSAndVTFMipOrderForXBOX(bool padded, std::span<const std::byte> imageData, ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount, bool& ok) {
	std::vector<std::byte> reorderedImageData;
	reorderedImageData.resize(ImageFormatDetails::getDataLengthXBOX(padded, format, mipCount, frameCount, faceCount, width, height, sliceCount));
	BufferStream reorderedStream{reorderedImageData};

	if constexpr (ConvertingFromDDS) {
		for (int i = mipCount - 1; i >= 0; i--) {
			for (int j = 0; j < frameCount; j++) {
				for (int k = 0; k < faceCount; k++) {
					for (int l = 0; l < sliceCount; l++) {
						uint32_t oldOffset, length;
						if (!ImageFormatDetails::getDataPositionXbox(oldOffset, length, padded, format, i, mipCount, j, frameCount, k, faceCount, width, height, l, sliceCount)) {
							ok = false;
							return {};
						}
						reorderedStream << imageData.subspan(oldOffset, length);
					}
				}
			}
		}
	} else {
		for (int j = 0; j < frameCount; j++) {
			for (int k = 0; k < faceCount; k++) {
				for (int i = 0; i < mipCount; i++) {
					for (int l = 0; l < sliceCount; l++) {
						uint32_t oldOffset, length;
						if (!ImageFormatDetails::getDataPosition(oldOffset, length, format, i, mipCount, j, frameCount, k, faceCount, width, height, l, sliceCount)) {
							ok = false;
							return {};
						}
						reorderedStream << imageData.subspan(oldOffset, length);
					}
				}
			}
			if (padded) {
				reorderedStream.pad(math::paddingForAlignment(512, reorderedStream.tell()));
			}
		}
	}

	ok = true;
	return reorderedImageData;
}

} // namespace

Resource::ConvertedData Resource::convertData() const {
	switch (this->type) {
		case TYPE_PARTICLE_SHEET_DATA:
			if (this->data.size() <= sizeof(uint32_t)) {
				return {};
			}
			return SHT{{reinterpret_cast<const std::byte*>(this->data.data()) + sizeof(uint32_t), *reinterpret_cast<const uint32_t*>(this->data.data())}};
		case TYPE_CRC:
		case TYPE_EXTENDED_FLAGS:
			if (this->data.size() != sizeof(uint32_t)) {
				return {};
			}
			return *reinterpret_cast<const uint32_t*>(this->data.data());
		case TYPE_LOD_CONTROL_INFO:
			if (this->data.size() != sizeof(uint32_t)) {
				return {};
			}
			return std::make_tuple(
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 0),
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 1),
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 2),
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 3));
		case TYPE_KEYVALUES_DATA:
			if (this->data.size() <= sizeof(uint32_t)) {
				return "";
			}
			return std::string(reinterpret_cast<const char*>(this->data.data()) + sizeof(uint32_t), *reinterpret_cast<const uint32_t*>(this->data.data()));
		case TYPE_HOTSPOT_DATA:
			if (this->data.size() <= sizeof(uint32_t)) {
				return {};
			}
			return HOT{{reinterpret_cast<const std::byte*>(this->data.data()) + sizeof(uint32_t), *reinterpret_cast<const uint32_t*>(this->data.data())}};
		case TYPE_AUX_COMPRESSION:
			if (this->data.size() <= sizeof(uint32_t) || this->data.size() % sizeof(uint32_t) != 0) {
				return {};
			}
			return std::span{reinterpret_cast<uint32_t*>(this->data.data()), this->data.size() / 4};
		default:
			break;
	}
	return {};
}

VTF::VTF() {
	this->opened = true;
}

VTF::VTF(std::vector<std::byte>&& vtfData, bool parseHeaderOnly)
		: data(std::move(vtfData)) {
	BufferStreamReadOnly stream{this->data};

	if (const auto signature = stream.read<uint32_t>(); signature == VTF_SIGNATURE) {
		stream >> this->platform;
		if (this->platform != PLATFORM_PC) {
			return;
		}
		stream >> this->version;
		if (this->version > 6) {
			return;
		}
	} else if (signature == VTFX_SIGNATURE || signature == VTF3_SIGNATURE) {
		stream.set_big_endian(true);
		stream >> this->platform;
		if (this->platform != PLATFORM_X360 && this->platform != PLATFORM_PS3_ORANGEBOX && this->platform != PLATFORM_PS3_PORTAL2) {
			return;
		}
		stream >> this->version;
		if (this->version != 8) {
			return;
		}
		// Now fix up the actual version as it would be on PC
		if (signature == VTF3_SIGNATURE) {
			this->platform = PLATFORM_PS3_PORTAL2;
			this->version = 5;
		} else {
			this->version = 4;
		}
	} else if (signature == XTF_SIGNATURE) {
		stream >> this->platform;
		if (this->platform != PLATFORM_XBOX) {
			return;
		}
		stream >> this->version;
		if (this->version != 0) {
			return;
		}
		// Now fix up the actual version as it would be on PC
		this->version = 2;
	} else {
		return;
	}

	const auto headerSize = stream.read<uint32_t>();

	const auto readResources = [this, &stream](uint32_t resourceCount) {
		// Read resource header info
		this->resources.reserve(resourceCount);
		for (int i = 0; i < resourceCount; i++) {
			auto& [type, flags_, data_] = this->resources.emplace_back();

			auto typeAndFlags = stream.read<uint32_t>();
			if (stream.is_big_endian()) {
				// This field is little-endian
				BufferStream::swap_endian(&typeAndFlags);
			}
			type = static_cast<Resource::Type>(typeAndFlags & 0xffffff); // last 3 bytes
			flags_ = static_cast<Resource::Flags>(typeAndFlags >> 24); // first byte
			data_ = stream.read_span<std::byte>(4);

			if (stream.is_big_endian() && !(flags_ & Resource::FLAG_LOCAL_DATA)) {
				BufferStream::swap_endian(reinterpret_cast<uint32_t*>(data_.data()));
			}
		}

		// Sort resources by their offset, in case certain VTFs are written
		// weirdly and have resource data written out of order. So far I have
		// found only one VTF in an official Valve game where this is the case
		std::ranges::sort(this->resources, [](const Resource& lhs, const Resource& rhs) {
			if ((lhs.flags & Resource::FLAG_LOCAL_DATA) && (rhs.flags & Resource::FLAG_LOCAL_DATA)) {
				return lhs.type < rhs.type;
			}
			if ((lhs.flags & Resource::FLAG_LOCAL_DATA) && !(rhs.flags & Resource::FLAG_LOCAL_DATA)) {
				return true;
			}
			if (!(lhs.flags & Resource::FLAG_LOCAL_DATA) && (rhs.flags & Resource::FLAG_LOCAL_DATA)) {
				return false;
			}
			return *reinterpret_cast<uint32_t*>(lhs.data.data()) < *reinterpret_cast<uint32_t*>(rhs.data.data());
		});

		// Fix up data spans to point to the actual data
		Resource* lastResource = nullptr;
		for (auto& resource : this->resources) {
			if (!(resource.flags & Resource::FLAG_LOCAL_DATA)) {
				if (lastResource) {
					const auto lastOffset = *reinterpret_cast<uint32_t*>(lastResource->data.data());
					const auto currentOffset = *reinterpret_cast<uint32_t*>(resource.data.data());
					const auto curPos = stream.tell();
					stream.seek(lastOffset);
					lastResource->data = stream.read_span<std::byte>(currentOffset - lastOffset);
					stream.seek(static_cast<int64_t>(curPos));
				}
				lastResource = &resource;
			}
		}
		if (lastResource) {
			const auto offset = *reinterpret_cast<uint32_t*>(lastResource->data.data());
			const auto curPos = stream.tell();
			stream.seek(offset);
			lastResource->data = stream.read_span<std::byte>(stream.size() - offset);
			stream.seek(static_cast<int64_t>(curPos));
		}
	};

	// HACK: a couple tweaks to fix engine bugs or branch differences
	const auto postReadTransform = [this] {
		// Change the format to DXT1_ONE_BIT_ALPHA to get compressonator to recognize it.
		// No source game recognizes this format, so we will do additional transform in bake back to DXT1.
		// We also need to check MULTI_BIT_ALPHA flag because stupid third party tools will sometimes set it???
		if (this->format == ImageFormat::DXT1 && this->flags & (FLAG_ONE_BIT_ALPHA | FLAG_MULTI_BIT_ALPHA)) {
			this->format = ImageFormat::DXT1_ONE_BIT_ALPHA;
		}
		// If the version is below 7.5, NV_NULL / ATI2N / ATI1N are at different positions in the enum.
		// It is safe to do this because these formats didn't exist before v7.5.
		if (this->version < 5 && (this->format == ImageFormat::RGBA1010102 || this->format == ImageFormat::BGRA1010102 || this->format == ImageFormat::R16F)) {
			this->format = static_cast<ImageFormat>(static_cast<int32_t>(this->format) - 3);
		}
	};

	switch (this->platform) {
		case PLATFORM_UNKNOWN:
			return;
		case PLATFORM_PC: {
			stream
				.read(this->width)
				.read(this->height)
				.read(this->flags)
				.read(this->frameCount)
				.read(this->startFrame)
				.skip(4)
				.read(this->reflectivity[0])
				.read(this->reflectivity[1])
				.read(this->reflectivity[2])
				.skip(4)
				.read(this->bumpMapScale)
				.read(this->format)
				.read(this->mipCount);

			postReadTransform();

			// This will always be DXT1
			stream.skip<ImageFormat>();
			stream >> this->thumbnailWidth >> this->thumbnailHeight;
			if (this->thumbnailWidth == 0 || this->thumbnailHeight == 0) {
				this->thumbnailFormat = ImageFormat::EMPTY;
			} else {
				this->thumbnailFormat = ImageFormat::DXT1;
			}

			if (this->version < 2) {
				this->sliceCount = 1;
			} else {
				stream.read(this->sliceCount);
			}

			if (parseHeaderOnly) {
				this->opened = true;
				return;
			}

			if (this->version >= 3) {
				stream.skip(3);
				auto resourceCount = stream.read<uint32_t>();
				stream.skip(8);
				readResources(resourceCount);

				this->opened = stream.tell() == headerSize;

				if (this->opened && this->version >= 6) {
					const auto* auxResource = this->getResource(Resource::TYPE_AUX_COMPRESSION);
					const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA);
					if (auxResource && imageResource) {
						if (auxResource->getDataAsAuxCompressionLevel() != 0) {
							const auto faceCount = this->getFaceCount();
							std::vector<std::byte> decompressedImageData(ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, faceCount, this->width, this->height, this->sliceCount));
							uint32_t oldOffset = 0;
							for (int i = this->mipCount - 1; i >= 0; i--) {
								for (int j = 0; j < this->frameCount; j++) {
									for (int k = 0; k < faceCount; k++) {
										uint32_t oldLength = auxResource->getDataAsAuxCompressionLength(i, this->mipCount, j, this->frameCount, k, faceCount);
										if (uint32_t newOffset, newLength; ImageFormatDetails::getDataPosition(newOffset, newLength, this->format, i, this->mipCount, j, this->frameCount, k, faceCount, this->width, this->height, 0, this->getSliceCount())) {
											// Keep in mind that slices are compressed together
											mz_ulong decompressedImageDataSize = newLength * this->sliceCount;
											switch (auxResource->getDataAsAuxCompressionMethod()) {
												using enum CompressionMethod;
												case DEFLATE:
													if (mz_uncompress(reinterpret_cast<unsigned char*>(decompressedImageData.data() + newOffset), &decompressedImageDataSize, reinterpret_cast<const unsigned char*>(imageResource->data.data() + oldOffset), oldLength) != MZ_OK) {
														this->opened = false;
														return;
													}
													break;
												case ZSTD:
													if (auto decompressedSize = ZSTD_decompress(reinterpret_cast<unsigned char*>(decompressedImageData.data() + newOffset), decompressedImageDataSize, reinterpret_cast<const unsigned char*>(imageResource->data.data() + oldOffset), oldLength); ZSTD_isError(decompressedSize) || decompressedSize != decompressedImageDataSize) {
														this->opened = false;
														return;
													}
													break;
												case CONSOLE_LZMA:
													// Shouldn't be here!
													SOURCEPP_DEBUG_BREAK;
													break;
											}
										}
										oldOffset += oldLength;
									}
								}
							}
							this->setResourceInternal(Resource::TYPE_IMAGE_DATA, decompressedImageData);
						}
					}
				}
			} else {
				stream.skip(math::paddingForAlignment(16, stream.tell()));
				this->opened = stream.tell() == headerSize;

				this->resources.reserve(2);

				if (this->hasThumbnailData()) {
					this->resources.push_back({
						.type = Resource::TYPE_THUMBNAIL_DATA,
						.flags = Resource::FLAG_NONE,
						.data = stream.read_span<std::byte>(ImageFormatDetails::getDataLength(this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight)),
					});
				}
				if (this->hasImageData()) {
					this->resources.push_back({
						.type = Resource::TYPE_IMAGE_DATA,
						.flags = Resource::FLAG_NONE,
						.data = stream.read_span<std::byte>(stream.size() - stream.tell()),
					});
				}
			}

			if (const auto* resource = this->getResource(Resource::TYPE_AUX_COMPRESSION)) {
				this->compressionLevel = resource->getDataAsAuxCompressionLevel();
				this->compressionMethod = resource->getDataAsAuxCompressionMethod();
				this->removeResourceInternal(Resource::TYPE_AUX_COMPRESSION);
			}
			return;
		}
		case PLATFORM_XBOX: {
			if (this->platform == PLATFORM_XBOX) {
				uint16_t preloadSize = 0, imageOffset = 0;
				stream
					.read(this->flags)
					.read(this->width)
					.read(this->height)
					.read(this->sliceCount)
					.read(this->frameCount)
					.read(preloadSize)
					.read(imageOffset)
					.read(this->reflectivity[0])
					.read(this->reflectivity[1])
					.read(this->reflectivity[2])
					.read(this->bumpMapScale)
					.read(this->format)
					.read(this->thumbnailWidth)
					.read(this->thumbnailHeight)
					.read(this->fallbackWidth)
					.read(this->fallbackHeight)
					.read(this->xboxMipScale)
					.skip<uint8_t>(); // padding

				const bool headerSizeIsAccurate = stream.tell() == headerSize;

				this->mipCount = (this->flags & FLAG_NO_MIP) ? 1 : ImageDimensions::getActualMipCountForDimsOnConsole(this->width, this->height);
				this->fallbackMipCount = (this->flags & FLAG_NO_MIP) ? 1 : ImageDimensions::getActualMipCountForDimsOnConsole(this->fallbackWidth, this->fallbackHeight);

				// Can't use VTF::getFaceCount yet because there's no image data
				const auto faceCount = (this->flags & FLAG_ENVMAP) ? 6 : 1;

				if (this->thumbnailWidth == 0 || this->thumbnailHeight == 0) {
					this->thumbnailFormat = ImageFormat::EMPTY;
				} else {
					this->thumbnailFormat = ImageFormat::RGB888;
					this->resources.push_back({
						.type = Resource::TYPE_THUMBNAIL_DATA,
						.flags = Resource::FLAG_NONE,
						.data = stream.read_span<std::byte>(ImageFormatDetails::getDataLength(this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight)),
					});
				}

				bool ok;
				const auto fallbackSize = ImageFormatDetails::getDataLengthXBOX(false, this->format, this->fallbackMipCount, this->frameCount, faceCount, this->fallbackWidth, this->fallbackHeight);
				auto reorderedFallbackData = ::convertBetweenDDSAndVTFMipOrderForXBOX<true>(false, stream.read_span<std::byte>(fallbackSize), this->format, this->fallbackMipCount, this->frameCount, faceCount, this->fallbackWidth, this->fallbackHeight, 1, ok);
				if (!ok) {
					this->opened = false;
					return;
				}
				::swapImageDataEndianForConsole<true>(reorderedFallbackData, this->format, this->fallbackMipCount, this->frameCount, faceCount, this->fallbackWidth, this->fallbackHeight, 1, this->platform);

				// todo(xtf): what about the palette?

				if (stream.tell() != preloadSize) {
					this->opened = false;
					return;
				}

				this->opened = headerSizeIsAccurate;
				if (parseHeaderOnly) {
					return;
				}

				const auto imageSize = ImageFormatDetails::getDataLengthXBOX(true, this->format, this->mipCount, this->frameCount, faceCount, this->width, this->height, this->sliceCount);
				auto reorderedImageData = ::convertBetweenDDSAndVTFMipOrderForXBOX<true>(true, stream.seek(imageOffset).read_span<std::byte>(imageSize), this->format, this->mipCount, this->frameCount, faceCount, this->width, this->height, this->sliceCount, ok);
				if (!ok) {
					this->opened = false;
					return;
				}
				::swapImageDataEndianForConsole<true>(reorderedImageData, this->format, this->mipCount, this->frameCount, faceCount, this->width, this->height, this->sliceCount, this->platform);

				// By this point we cannot use spans over data, it will change here
				this->setResourceInternal(Resource::TYPE_FALLBACK_DATA, reorderedFallbackData);
				this->setResourceInternal(Resource::TYPE_IMAGE_DATA, reorderedImageData);
				return;
			}
		}
		case PLATFORM_X360:
		case PLATFORM_PS3_ORANGEBOX:
		case PLATFORM_PS3_PORTAL2: {
			uint8_t resourceCount;
			stream
				.read(this->flags)
				.read(this->width)
				.read(this->height)
				.read(this->sliceCount)
				.read(this->frameCount)
				.skip<uint16_t>() // preload
				.skip<uint8_t>() // skip high mip levels
				.read(resourceCount)
				.read(this->reflectivity[0])
				.read(this->reflectivity[1])
				.read(this->reflectivity[2])
				.read(this->bumpMapScale)
				.read(this->format)
				.skip<math::Vec4ui8>() // lowResImageSample (replacement for thumbnail resource, linear color pixel)
				.skip<uint32_t>(); // compressedLength

			postReadTransform();

			// Align to 16 bytes
			if (this->platform == PLATFORM_PS3_PORTAL2) {
				stream.skip<uint32_t>();
			}

			this->mipCount = (this->flags & FLAG_NO_MIP) ? 1 : ImageDimensions::getActualMipCountForDimsOnConsole(this->width, this->height);

			if (parseHeaderOnly) {
				this->opened = true;
				return;
			}

			this->resources.reserve(resourceCount);
			readResources(resourceCount);

			this->opened = stream.tell() == headerSize;

			// The resources vector isn't modified by setResourceInternal when we're not adding a new one, so this is fine
			for (const auto& resource : this->resources) {
				// Decompress LZMA resources
				if (BufferStreamReadOnly rsrcStream{resource.data.data(), resource.data.size()}; rsrcStream.read<uint32_t>() == compression::VALVE_LZMA_SIGNATURE) {
					if (auto decompressedData = compression::decompressValveLZMA(resource.data)) {
						this->setResourceInternal(resource.type, *decompressedData);

						if (resource.type == Resource::TYPE_IMAGE_DATA) {
							// Do this here because compressionLength in header can be garbage on PS3 orange box
							this->compressionMethod = CompressionMethod::CONSOLE_LZMA;
						}
					}
				}

				if (resource.type == Resource::TYPE_THUMBNAIL_DATA) {
					::swapImageDataEndianForConsole<true>(resource.data, this->thumbnailFormat, 1, 1, 1, this->thumbnailWidth, this->thumbnailHeight, 1, this->platform);
				} else if (resource.type == Resource::TYPE_IMAGE_DATA) {
					::swapImageDataEndianForConsole<true>(resource.data, this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount, this->platform);
				} else if (!(resource.flags & Resource::FLAG_LOCAL_DATA) && resource.data.size() >= sizeof(uint32_t)) {
					BufferStream::swap_endian(reinterpret_cast<uint32_t*>(resource.data.data()));
				}
			}
			return;
		}
	}
}

VTF::VTF(std::span<const std::byte> vtfData, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{vtfData.begin(), vtfData.end()}, parseHeaderOnly) {}

VTF::VTF(const std::string& vtfPath, bool parseHeaderOnly)
		: VTF(fs::readFileBuffer(vtfPath), parseHeaderOnly) {}

VTF::VTF(const VTF& other) {
	*this = other;
}

VTF& VTF::operator=(const VTF& other) {
	this->opened = other.opened;
	this->data = other.data;
	this->version = other.version;
	this->width = other.width;
	this->height = other.height;
	this->flags = other.flags;
	this->frameCount = other.frameCount;
	this->startFrame = other.startFrame;
	this->reflectivity = other.reflectivity;
	this->bumpMapScale = other.bumpMapScale;
	this->format = other.format;
	this->mipCount = other.mipCount;
	this->thumbnailFormat = other.thumbnailFormat;
	this->thumbnailWidth = other.thumbnailWidth;
	this->thumbnailHeight = other.thumbnailHeight;
	this->fallbackWidth = other.fallbackWidth;
	this->fallbackHeight = other.fallbackHeight;
	this->fallbackMipCount = other.fallbackMipCount;
	this->xboxMipScale = other.xboxMipScale;
	this->sliceCount = other.sliceCount;

	this->resources.clear();
	for (const auto& [otherType, otherFlags, otherData] : other.resources) {
		auto& [type, flags_, data_] = this->resources.emplace_back();
		type = otherType;
		flags_ = otherFlags;
		data_ = {this->data.data() + (otherData.data() - other.data.data()), otherData.size()};
	}

	this->platform = other.platform;
	this->compressionLevel = other.compressionLevel;
	this->compressionMethod = other.compressionMethod;
	this->imageWidthResizeMethod = other.imageWidthResizeMethod;
	this->imageHeightResizeMethod = other.imageHeightResizeMethod;

	return *this;
}

VTF::operator bool() const {
	return this->opened;
}

bool VTF::createInternal(VTF& writer, CreationOptions options) {
	bool out = true;
	if (writer.hasImageData() && (options.invertGreenChannel || options.gammaCorrection != 1.f)) {
		for (int i = 1; i < writer.mipCount; i++) {
			for (int j = 0; j < writer.frameCount; j++) {
				for (int k = 0; k < writer.getFaceCount(); k++) {
					for (int l = 0; l < writer.sliceCount; l++) {
						if (options.invertGreenChannel && !writer.setImage(ImageConversion::invertGreenChannelForImageData(writer.getImageDataRaw(i, j, k, l), writer.getFormat(), writer.getWidth(i), writer.getHeight(i)), writer.getFormat(), writer.getWidth(i), writer.getHeight(i), ImageConversion::ResizeFilter::DEFAULT, i, j, k, l)) {
							out = false;
						}
						if (options.gammaCorrection != 1.f && !writer.setImage(ImageConversion::gammaCorrectImageData(writer.getImageDataRaw(i, j, k, l), writer.getFormat(), writer.getWidth(i), writer.getHeight(i), options.gammaCorrection), writer.getFormat(), writer.getWidth(i), writer.getHeight(i), ImageConversion::ResizeFilter::DEFAULT, i, j, k, l)) {
							out = false;
						}
					}
				}
			}
		}
	}
	writer.setPlatform(options.platform);
	if (options.computeReflectivity) {
		writer.computeReflectivity();
	}
	if (options.initialFrameCount > 1 || options.isCubeMap || options.initialSliceCount > 1) {
		if (!writer.setFrameFaceAndSliceCount(options.initialFrameCount, options.isCubeMap, options.initialSliceCount)) {
			out = false;
		}
	}
	writer.setStartFrame(options.startFrame);
	writer.setBumpMapScale(options.bumpMapScale);
	if (options.computeThumbnail) {
		writer.computeThumbnail();
	}
	if (options.outputFormat == VTF::FORMAT_UNCHANGED) {
		options.outputFormat = writer.getFormat();
	} else if (options.outputFormat == VTF::FORMAT_DEFAULT) {
		options.outputFormat = VTF::getDefaultCompressedFormat(writer.getFormat(), writer.getVersion(), options.isCubeMap);
	}
	if (options.computeMips) {
		if (const auto recommendedMipCount = ImageDimensions::getRecommendedMipCountForDims(options.outputFormat, writer.getWidth(), writer.getHeight()); recommendedMipCount > 1) {
			if (!writer.setMipCount(recommendedMipCount)) {
				out = false;
			}
			writer.computeMips(options.filter);
		}
	}
	writer.setFormat(options.outputFormat, ImageConversion::ResizeFilter::DEFAULT, options.compressedFormatQuality);
	if (options.computeTransparencyFlags) {
		writer.computeTransparencyFlags();
	}
	writer.setCompressionLevel(options.compressionLevel);
	writer.setCompressionMethod(options.compressionMethod);
	writer.setXBOXMipScale(options.xboxMipScale);
	return out;
}

bool VTF::create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, const CreationOptions& options) {
	VTF writer;
	writer.setVersion(options.version);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	if (!writer.setImage(imageData, format, width, height, options.filter)) {
		return false;
	}
	if (!createInternal(writer, options)) {
		return false;
	}
	return writer.bake(vtfPath);
}

bool VTF::create(ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, const CreationOptions& options) {
	std::vector<std::byte> imageData;
	imageData.resize(static_cast<uint32_t>(width) * height * ImageFormatDetails::bpp(format) / 8);
	return create(imageData, format, width, height, vtfPath, options);
}

VTF VTF::create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options) {
	VTF writer;
	writer.setVersion(options.version);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	writer.setImage(imageData, format, width, height, options.filter);
	createInternal(writer, options);
	return writer;
}

VTF VTF::create(ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options) {
	std::vector<std::byte> imageData;
	imageData.resize(static_cast<uint32_t>(width) * height * ImageFormatDetails::bpp(format) / 8);
	return create(imageData, format, width, height, options);
}

bool VTF::create(const std::string& imagePath, const std::string& vtfPath, const CreationOptions& options) {
	VTF writer;
	writer.setVersion(options.version);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	if (!writer.setImage(imagePath, options.filter)) {
		return false;
	}
	if (!createInternal(writer, options)) {
		return false;
	}
	return writer.bake(vtfPath);
}

VTF VTF::create(const std::string& imagePath, const CreationOptions& options) {
	VTF writer;
	writer.setVersion(options.version);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	writer.setImage(imagePath, options.filter);
	createInternal(writer, options);
	return writer;
}

VTF::Platform VTF::getPlatform() const {
	return this->platform;
}

void VTF::setPlatform(Platform newPlatform) {
	if (this->platform == newPlatform) {
		return;
	}

	// hack to allow VTF::setVersion to work
	const auto oldPlatform = this->platform;
	this->platform = PLATFORM_PC;
	switch (newPlatform) {
		case PLATFORM_UNKNOWN:
		case PLATFORM_PC:
			break;
		case PLATFORM_XBOX:
			this->setVersion(2);
			break;
		case PLATFORM_X360:
		case PLATFORM_PS3_ORANGEBOX:
			this->setVersion(4);
			break;
		case PLATFORM_PS3_PORTAL2:
			this->setVersion(5);
			break;
	}
	this->platform = newPlatform;

	// Update flags
	if (this->platform == PLATFORM_XBOX || newPlatform == PLATFORM_XBOX) {
		this->removeFlags(VTF::FLAGS_MASK_XBOX);
	}

	// XBOX stores thumbnail as single RGB888 pixel, but we assume thumbnail is DXT1 on other platforms
	if (this->hasThumbnailData()) {
		if (this->platform == PLATFORM_XBOX) {
			this->thumbnailFormat = ImageFormat::RGB888;
			this->thumbnailWidth = 1;
			this->thumbnailHeight = 1;
			std::array newThumbnail{
				static_cast<std::byte>(static_cast<uint8_t>(std::clamp(this->reflectivity[0], 0.f, 1.f) * 255.f)),
				static_cast<std::byte>(static_cast<uint8_t>(std::clamp(this->reflectivity[1], 0.f, 1.f) * 255.f)),
				static_cast<std::byte>(static_cast<uint8_t>(std::clamp(this->reflectivity[2], 0.f, 1.f) * 255.f)),
			};
			this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, newThumbnail);
		} else if (oldPlatform == PLATFORM_XBOX) {
			this->thumbnailFormat = ImageFormat::EMPTY;
			this->thumbnailWidth = 0;
			this->thumbnailHeight = 0;
		}
	}

	// Add/remove fallback data for XBOX
	if (this->platform != PLATFORM_XBOX && this->hasFallbackData()) {
		this->removeFallback();
	} else if (this->platform == PLATFORM_XBOX) {
		this->computeFallback();
	}

	this->setCompressionMethod(this->compressionMethod);

	if (this->platform != PLATFORM_PC) {
		const auto recommendedCount = (this->flags & VTF::FLAG_NO_MIP) ? 1 : ImageDimensions::getActualMipCountForDimsOnConsole(this->width, this->height);
		if (this->mipCount != recommendedCount) {
			this->setMipCount(recommendedCount);
		}
	} else if (oldPlatform != PLATFORM_PC) {
		const auto recommendedMipCount = ImageDimensions::getRecommendedMipCountForDims(this->format, this->width, this->height);
		if (this->mipCount > recommendedMipCount) {
			this->setMipCount(recommendedMipCount);
		}
	}
}

uint32_t VTF::getVersion() const {
	return this->version;
}

void VTF::setVersion(uint32_t newVersion) {
	if (this->platform != PLATFORM_PC) {
		return;
	}
	if (this->hasImageData()) {
		auto faceCount = this->getFaceCount();
		if (faceCount == 7 && (newVersion < 1 || newVersion > 4)) {
			faceCount = 6;
		}
		this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, faceCount, this->sliceCount);
	}

	// Fix up flags
	const bool srgb = this->isSRGB();
	if ((this->version < 2 && newVersion >= 2) || (this->version >= 2 && newVersion < 2)) {
		this->removeFlags(VTF::FLAGS_MASK_V2);
	}
	if ((this->version < 3 && newVersion >= 3) || (this->version >= 3 && newVersion < 3)) {
		this->removeFlags(VTF::FLAGS_MASK_V3);
	}
	if ((this->version < 4 && newVersion >= 4) || (this->version >= 4 && newVersion < 4)) {
		this->removeFlags(VTF::FLAGS_MASK_V4);
		this->removeFlags(VTF::FLAGS_MASK_V4_TF2);
	}
	if ((this->version < 5 && newVersion >= 5) || (this->version >= 5 && newVersion < 5)) {
		this->removeFlags(VTF::FLAGS_MASK_V5);
		this->removeFlags(VTF::FLAGS_MASK_V5_CSGO);
	}
	this->setSRGB(srgb);

	this->version = newVersion;
}

ImageConversion::ResizeMethod VTF::getImageWidthResizeMethod() const {
	return this->imageWidthResizeMethod;
}

ImageConversion::ResizeMethod VTF::getImageHeightResizeMethod() const {
	return this->imageHeightResizeMethod;
}

void VTF::setImageResizeMethods(ImageConversion::ResizeMethod imageWidthResizeMethod_, ImageConversion::ResizeMethod imageHeightResizeMethod_) {
	this->imageWidthResizeMethod = imageWidthResizeMethod_;
	this->imageHeightResizeMethod = imageHeightResizeMethod_;
}

void VTF::setImageWidthResizeMethod(ImageConversion::ResizeMethod imageWidthResizeMethod_) {
	this->imageWidthResizeMethod = imageWidthResizeMethod_;
}

void VTF::setImageHeightResizeMethod(ImageConversion::ResizeMethod imageHeightResizeMethod_) {
	this->imageHeightResizeMethod = imageHeightResizeMethod_;
}

uint16_t VTF::getWidth(uint8_t mip) const {
	return mip > 0 ? ImageDimensions::getMipDim(mip, this->width) : this->width;
}

uint16_t VTF::getHeight(uint8_t mip) const {
	return mip > 0 ? ImageDimensions::getMipDim(mip, this->height) : this->height;
}

void VTF::setSize(uint16_t newWidth, uint16_t newHeight, ImageConversion::ResizeFilter filter) {
	if (newWidth == 0 || newHeight == 0) {
		this->format = ImageFormat::EMPTY;
		this->width = 0;
		this->height = 0;
		this->removeResourceInternal(Resource::TYPE_IMAGE_DATA);
		return;
	}

	ImageConversion::setResizedDims(newWidth, this->imageWidthResizeMethod, newHeight, this->imageHeightResizeMethod);
	if (this->hasImageData()) {
		if (this->width == newWidth && this->height == newHeight) {
			return;
		}
		auto newMipCount = this->mipCount;
		if (this->platform == VTF::PLATFORM_PC) {
			if (const auto recommendedCount = ImageDimensions::getRecommendedMipCountForDims(this->format, newWidth, newHeight); newMipCount > recommendedCount) {
				newMipCount = recommendedCount;
			}
		} else {
			newMipCount = (this->flags & VTF::FLAG_NO_MIP) ? 1 : ImageDimensions::getActualMipCountForDimsOnConsole(newWidth, newHeight);
		}
		this->regenerateImageData(this->format, newWidth, newHeight, newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount, filter);
	} else {
		this->format = ImageFormat::RGBA8888;
		this->mipCount = 1;
		this->frameCount = 1;
		this->flags &= ~FLAG_ENVMAP;
		this->width = newWidth;
		this->height = newHeight;
		this->sliceCount = 1;
		this->setResourceInternal(Resource::TYPE_IMAGE_DATA, std::vector<std::byte>(ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, 1, this->width, this->height, this->sliceCount)));
	}
}

uint32_t VTF::getFlags() const {
	return this->flags;
}

void VTF::setFlags(uint32_t flags_) {
	this->flags = (this->flags & FLAGS_MASK_INTERNAL) | (flags_ & ~FLAGS_MASK_INTERNAL);
}

void VTF::addFlags(uint32_t flags_) {
	this->flags |= flags_ & ~FLAGS_MASK_INTERNAL;
}

void VTF::removeFlags(uint32_t flags_) {
	this->flags &= ~flags_ | FLAGS_MASK_INTERNAL;
}

bool VTF::isSRGB() const {
	return !ImageFormatDetails::large(this->format) && (this->version < 4 ? false : this->version < 5 ? this->flags & FLAG_V4_SRGB : this->flags & FLAG_V5_PWL_CORRECTED || this->flags & FLAG_V5_SRGB);
}

void VTF::setSRGB(bool srgb) {
	if (srgb) {
		if (this->version >= 5) {
			this->addFlags(FLAG_V5_SRGB);
		} else if (this->version >= 4) {
			this->addFlags(FLAG_V4_SRGB);
		}
	} else {
		if (this->version >= 5) {
			this->removeFlags(FLAG_V5_PWL_CORRECTED | FLAG_V5_SRGB);
		} else if (this->version >= 4) {
			this->removeFlags(FLAG_V4_SRGB);
		}
	}
}

void VTF::computeTransparencyFlags() {
	if (ImageFormatDetails::transparent(this->format)) {
		if (ImageFormatDetails::decompressedAlpha(this->format) > 1) {
			this->flags &= ~VTF::FLAG_ONE_BIT_ALPHA;
			this->flags |= VTF::FLAG_MULTI_BIT_ALPHA;
		} else {
			this->flags |= VTF::FLAG_ONE_BIT_ALPHA;
			this->flags &= ~VTF::FLAG_MULTI_BIT_ALPHA;
		}
	} else {
		this->flags &= ~VTF::FLAG_ONE_BIT_ALPHA;
		this->flags &= ~VTF::FLAG_MULTI_BIT_ALPHA;
	}
}

ImageFormat VTF::getDefaultCompressedFormat(ImageFormat inputFormat, uint32_t version, bool isCubeMap) {
	if (version >= 6) {
		if (isCubeMap) {
			return ImageFormat::BC6H;
		}
		return ImageFormat::BC7;
	}
	if (ImageFormatDetails::decompressedAlpha(inputFormat) > 0) {
		return ImageFormat::DXT5;
	}
	return ImageFormat::DXT1;
}

ImageFormat VTF::getFormat() const {
	return this->format;
}

void VTF::setFormat(ImageFormat newFormat, ImageConversion::ResizeFilter filter, float quality) {
	if (newFormat == VTF::FORMAT_UNCHANGED || newFormat == this->format) {
		return;
	}
	if (newFormat == VTF::FORMAT_DEFAULT) {
		newFormat = VTF::getDefaultCompressedFormat(this->format, this->version, this->getFaceCount() > 1);
	}
	if (!this->hasImageData()) {
		this->format = newFormat;
		return;
	}
	const auto oldFormat = this->format;
	auto newMipCount = this->mipCount;
	if (const auto recommendedCount = ImageDimensions::getRecommendedMipCountForDims(newFormat, this->width, this->height); newMipCount > recommendedCount) {
		newMipCount = recommendedCount;
	}
	if (ImageFormatDetails::compressed(newFormat)) {
		this->regenerateImageData(newFormat, this->width + math::paddingForAlignment(4, this->width), this->height + math::paddingForAlignment(4, this->height), newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount, filter, quality);
	} else {
		this->regenerateImageData(newFormat, this->width, this->height, newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount, filter, quality);
	}

	if (const auto* fallbackResource = this->getResource(Resource::TYPE_FALLBACK_DATA)) {
		const auto fallbackConverted = ImageConversion::convertSeveralImageDataToFormat(fallbackResource->data, oldFormat, this->format, ImageDimensions::getActualMipCountForDimsOnConsole(this->fallbackWidth, this->fallbackHeight), this->frameCount, this->getFaceCount(), this->fallbackWidth, this->fallbackHeight, 1, quality);
		this->setResourceInternal(Resource::TYPE_FALLBACK_DATA, fallbackConverted);
	}
}

uint8_t VTF::getMipCount() const {
	return this->mipCount;
}

bool VTF::setMipCount(uint8_t newMipCount) {
	if (!this->hasImageData()) {
		return false;
	}
	if (this->platform != PLATFORM_PC && newMipCount > 1) {
		newMipCount = ImageDimensions::getActualMipCountForDimsOnConsole(this->width, this->height);
	} else if (const auto recommended = ImageDimensions::getRecommendedMipCountForDims(this->format, this->width, this->height); newMipCount > recommended) {
		newMipCount = recommended;
		if (newMipCount == 1) {
			return false;
		}
	}
	this->regenerateImageData(this->format, this->width, this->height, newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount);

	if (this->hasFallbackData() && ((this->mipCount > 1 && this->fallbackMipCount <= 1) || (this->mipCount == 1 && this->fallbackMipCount > 1))) {
		this->removeFallback();
		this->computeFallback();
	}
	return true;
}

bool VTF::setRecommendedMipCount() {
	if (this->platform == VTF::PLATFORM_PC) {
		return this->setMipCount(ImageDimensions::getRecommendedMipCountForDims(this->format, this->width, this->height));
	}
	return this->setMipCount(ImageDimensions::getActualMipCountForDimsOnConsole(this->width, this->height));
}

void VTF::computeMips(ImageConversion::ResizeFilter filter) {
	auto* imageResource = this->getResourceInternal(Resource::TYPE_IMAGE_DATA);
	if (!imageResource || !this->hasImageData()) {
		return;
	}

	if (this->mipCount <= 1) {
		if (!this->setRecommendedMipCount() || this->mipCount <= 1) {
			return;
		}
	}

	auto* outputDataPtr = imageResource->data.data();
	const auto faceCount = this->getFaceCount();

#ifdef SOURCEPP_BUILD_WITH_THREADS
	std::vector<std::future<void>> futures;
	futures.reserve(this->frameCount * faceCount * this->sliceCount);
#endif
	for (int j = 0; j < this->frameCount; j++) {
		for (int k = 0; k < faceCount; k++) {
			for (int l = 0; l < this->sliceCount; l++) {
#ifdef SOURCEPP_BUILD_WITH_THREADS
				futures.push_back(std::async(std::launch::async, [this, filter, outputDataPtr, faceCount, j, k, l] {
#endif
					for (int i = 1; i < this->mipCount; i++) {
						auto mip = ImageConversion::resizeImageData(this->getImageDataRaw(i - 1, j, k, l), this->format, ImageDimensions::getMipDim(i - 1, this->width), ImageDimensions::getMipDim(i, this->width), ImageDimensions::getMipDim(i - 1, this->height), ImageDimensions::getMipDim(i, this->height), this->isSRGB(), filter);
						if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, i, this->mipCount, j, this->frameCount, k, faceCount, this->width, this->height, l, this->sliceCount) && mip.size() == length) {
							std::memcpy(outputDataPtr + offset, mip.data(), length);
						}
					}
#ifdef SOURCEPP_BUILD_WITH_THREADS
				}));
				if (std::thread::hardware_concurrency() > 0 && futures.size() >= std::thread::hardware_concurrency()) {
					for (auto& future : futures) {
						future.get();
					}
					futures.clear();
				}
#endif
			}
		}
	}
#ifdef SOURCEPP_BUILD_WITH_THREADS
	for (auto& future : futures) {
		future.get();
	}
#endif
}

uint16_t VTF::getFrameCount() const {
	return this->frameCount;
}

bool VTF::setFrameCount(uint16_t newFrameCount) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, newFrameCount, this->getFaceCount(), this->sliceCount);
	return true;
}

uint8_t VTF::getFaceCount() const {
	if (!this->hasImageData()) {
		return 0;
	}
	const auto* image = this->getResource(Resource::TYPE_IMAGE_DATA);
	if (!image) {
		return 0;
	}
	if (!(this->flags & VTF::FLAG_ENVMAP)) {
		return 1;
	}
	if (this->version >= 6) {
		// All v7.6 VTFs are sane, and we need this special case to fix a bug in the parser where
		// it won't recognize cubemaps as cubemaps because the image resource is compressed!
		return 6;
	}
	const auto expectedLength = ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, 6, this->width, this->height, this->sliceCount);
	if (this->version >= 1 && this->version <= 4 && expectedLength < image->data.size()) {
		return 7;
	}
	if (expectedLength == image->data.size()) {
		return 6;
	}
	return 1;
}

bool VTF::setFaceCount(bool isCubemap) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, isCubemap ? ((this->version >= 1 && this->version <= 4) ? 7 : 6) : 1, this->sliceCount);
	return true;
}

uint16_t VTF::getSliceCount() const {
	return this->sliceCount;
}

bool VTF::setSliceCount(uint16_t newSliceCount) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, this->getFaceCount(), newSliceCount);
	return true;
}

bool VTF::setFrameFaceAndSliceCount(uint16_t newFrameCount, bool isCubemap, uint16_t newSliceCount) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, newFrameCount, isCubemap ? ((this->version >= 1 && this->version <= 4) ? 7 : 6) : 1, newSliceCount);
	return true;
}

uint16_t VTF::getStartFrame() const {
	return this->startFrame;
}

void VTF::setStartFrame(uint16_t newStartFrame) {
	this->startFrame = newStartFrame;
}

math::Vec3f VTF::getReflectivity() const {
	return this->reflectivity;
}

void VTF::setReflectivity(sourcepp::math::Vec3f newReflectivity) {
	this->reflectivity = newReflectivity;
}

void VTF::computeReflectivity() {
	static constexpr auto getReflectivityForImage = [](const VTF& vtf, uint16_t frame, uint8_t face, uint16_t slice) {
		static constexpr auto getReflectivityForPixel = [](const ImagePixel::RGBA8888* pixel) -> math::Vec3f {
			// http://markjstock.org/doc/gsd_talk_11_notes.pdf page 11
			math::Vec3f ref{static_cast<float>(pixel->r), static_cast<float>(pixel->g), static_cast<float>(pixel->b)};
			ref /= 255.f * 0.9f;
			ref[0] *= ref[0];
			ref[1] *= ref[1];
			ref[2] *= ref[2];
			return ref;
		};

		auto rgba8888Data = vtf.getImageDataAsRGBA8888(0, frame, face, slice);
		math::Vec3f out{};
		for (uint64_t i = 0; i < rgba8888Data.size(); i += 4) {
			out += getReflectivityForPixel(reinterpret_cast<ImagePixel::RGBA8888*>(rgba8888Data.data() + i));
		}
		return out / (rgba8888Data.size() / sizeof(ImagePixel::RGBA8888));
	};

	const auto faceCount = this->getFaceCount();

#ifdef SOURCEPP_BUILD_WITH_THREADS
	if (this->frameCount > 1 || faceCount > 1 || this->sliceCount > 1) {
		std::vector<std::future<math::Vec3f>> futures;
		futures.reserve(this->frameCount * faceCount * this->sliceCount);

		this->reflectivity = {};
		for (int j = 0; j < this->frameCount; j++) {
			for (int k = 0; k < faceCount; k++) {
				for (int l = 0; l < this->sliceCount; l++) {
					futures.push_back(std::async(std::launch::async, [this, j, k, l] {
						return getReflectivityForImage(*this, j, k, l);
					}));
					if (std::thread::hardware_concurrency() > 0 && futures.size() >= std::thread::hardware_concurrency()) {
						for (auto& future : futures) {
							this->reflectivity += future.get();
						}
						futures.clear();
					}
				}
			}
		}

		for (auto& future : futures) {
			this->reflectivity += future.get();
		}
		this->reflectivity /= this->frameCount * faceCount * this->sliceCount;
	} else {
		this->reflectivity = getReflectivityForImage(*this, 0, 0, 0);
	}
#else
	this->reflectivity = {};
	for (int j = 0; j < this->frameCount; j++) {
		for (int k = 0; k < faceCount; k++) {
			for (int l = 0; l < this->sliceCount; l++) {
				this->reflectivity += getReflectivityForImage(*this, j, k, l);
			}
		}
	}
	this->reflectivity /= this->frameCount * faceCount * this->sliceCount;
#endif
}

float VTF::getBumpMapScale() const {
	return this->bumpMapScale;
}

void VTF::setBumpMapScale(float newBumpMapScale) {
	this->bumpMapScale = newBumpMapScale;
}

ImageFormat VTF::getThumbnailFormat() const {
	return this->thumbnailFormat;
}

uint8_t VTF::getThumbnailWidth() const {
	return this->thumbnailWidth;
}

uint8_t VTF::getThumbnailHeight() const {
	return this->thumbnailHeight;
}

uint8_t VTF::getFallbackWidth() const {
	return this->fallbackWidth;
}

uint8_t VTF::getFallbackHeight() const {
	return this->fallbackHeight;
}

uint8_t VTF::getFallbackMipCount() const {
	return this->fallbackMipCount;
}

const std::vector<Resource>& VTF::getResources() const {
	return this->resources;
}

const Resource* VTF::getResource(Resource::Type type) const {
	for (const auto& resource : this->resources) {
		if (resource.type == type) {
			return &resource;
		}
	}
	return nullptr;
}

Resource* VTF::getResourceInternal(Resource::Type type) {
	for (auto& resource : this->resources) {
		if (resource.type == type) {
			return &resource;
		}
	}
	return nullptr;
}

void VTF::setResourceInternal(Resource::Type type, std::span<const std::byte> data_) {
	if (const auto* resource = this->getResource(type); resource && resource->data.size() == data_.size()) {
		std::memcpy(resource->data.data(), data_.data(), data_.size());
		return;
	}

	// Store resource data
	std::unordered_map<Resource::Type, std::pair<std::vector<std::byte>, uint64_t>> resourceData;
	for (const auto& [type_, flags_, dataSpan] : this->resources) {
		resourceData[type_] = {std::vector<std::byte>{dataSpan.begin(), dataSpan.end()}, 0};
	}

	// Set new resource
	if (data_.empty()) {
		resourceData.erase(type);
	} else {
		resourceData[type] = {{data_.begin(), data_.end()}, 0};
	}

	// Save the data
	this->data.clear();
	BufferStream writer{this->data};

	for (auto resourceType : Resource::getOrder()) {
		if (!resourceData.contains(resourceType)) {
			continue;
		}
		auto& [specificResourceData, offset] = resourceData[resourceType];
		if (resourceType == type) {
			Resource newResource{
				type,
				specificResourceData.size() <= sizeof(uint32_t) ? Resource::FLAG_LOCAL_DATA : Resource::FLAG_NONE,
				{this->data.data() + offset, specificResourceData.size()},
			};
			if (auto* resourcePtr = this->getResourceInternal(type)) {
				*resourcePtr = newResource;
			} else {
				this->resources.push_back(newResource);
			}
		} else if (!resourceData.contains(resourceType)) {
			continue;
		}
		offset = writer.tell();
		writer.write(specificResourceData);
	}
	this->data.resize(writer.size());

	for (auto& [type_, flags_, dataSpan] : this->resources) {
		if (resourceData.contains(type_)) {
			const auto& [specificResourceData, offset] = resourceData[type_];
			dataSpan = {this->data.data() + offset, specificResourceData.size()};
		}
	}
}

void VTF::removeResourceInternal(Resource::Type type) {
	std::erase_if(this->resources, [type](const Resource& resource) { return resource.type == type; });
}

void VTF::regenerateImageData(ImageFormat newFormat, uint16_t newWidth, uint16_t newHeight, uint8_t newMipCount, uint16_t newFrameCount, uint8_t newFaceCount, uint16_t newSliceCount, ImageConversion::ResizeFilter filter, float quality) {
	if (!newWidth)      { newWidth      = 1; }
	if (!newHeight)     { newHeight     = 1; }
	if (!newMipCount)   { newMipCount   = 1; }
	if (!newFrameCount) { newFrameCount = 1; }
	if (!newFaceCount)  { newFaceCount  = 1; }
	if (!newSliceCount) { newSliceCount = 1; }

	const auto faceCount = this->getFaceCount();
	if (this->format == newFormat && this->width == newWidth && this->height == newHeight && this->mipCount == newMipCount && this->frameCount == newFrameCount && faceCount == newFaceCount && this->sliceCount == newSliceCount) {
		return;
	}

	if (newMipCount > 1) {
		this->flags &= ~(VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD);
	} else {
		this->flags |= VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD;
	}

	std::vector<std::byte> newImageData;
	if (const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA); imageResource && this->hasImageData()) {
		if (this->format != newFormat && this->width == newWidth && this->height == newHeight && this->mipCount == newMipCount && this->frameCount == newFrameCount && faceCount == newFaceCount && this->sliceCount == newSliceCount) {
			newImageData = ImageConversion::convertSeveralImageDataToFormat(imageResource->data, this->format, newFormat, this->mipCount, this->frameCount, faceCount, this->width, this->height, this->sliceCount, quality);
		} else {
			newImageData.resize(ImageFormatDetails::getDataLength(this->format, newMipCount, newFrameCount, newFaceCount, newWidth, newHeight, newSliceCount));
			for (int i = newMipCount - 1; i >= 0; i--) {
				for (int j = 0; j < newFrameCount; j++) {
					for (int k = 0; k < newFaceCount; k++) {
						for (int l = 0; l < newSliceCount; l++) {
							if (i < this->mipCount && j < this->frameCount && k < faceCount && l < this->sliceCount) {
								auto imageSpan = this->getImageDataRaw(i, j, k, l);
								std::vector<std::byte> image{imageSpan.begin(), imageSpan.end()};
								if (this->width != newWidth || this->height != newHeight) {
									image = ImageConversion::resizeImageData(image, this->format, ImageDimensions::getMipDim(i, this->width), ImageDimensions::getMipDim(i, newWidth), ImageDimensions::getMipDim(i, this->height), ImageDimensions::getMipDim(i, newHeight), this->isSRGB(), filter);
								}
								if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, i, newMipCount, j, newFrameCount, k, newFaceCount, newWidth, newHeight, l, newSliceCount) && image.size() == length) {
									std::memcpy(newImageData.data() + offset, image.data(), length);
								}
							}
						}
					}
				}
			}
			if (this->format != newFormat) {
				newImageData = ImageConversion::convertSeveralImageDataToFormat(newImageData, this->format, newFormat, newMipCount, newFrameCount, newFaceCount, newWidth, newHeight, newSliceCount, quality);
			}
		}
	} else {
		newImageData.resize(ImageFormatDetails::getDataLength(newFormat, newMipCount, newFrameCount, newFaceCount, newWidth, newHeight, newSliceCount));
	}

	this->format = newFormat;
	this->width = newWidth;
	this->height = newHeight;
	this->mipCount = newMipCount;
	this->frameCount = newFrameCount;
	if (newFaceCount > 1) {
		this->flags |= FLAG_ENVMAP;
	} else {
		this->flags &= ~FLAG_ENVMAP;
	}
	this->sliceCount = newSliceCount;

	this->setResourceInternal(Resource::TYPE_IMAGE_DATA, newImageData);
}

std::vector<std::byte> VTF::getParticleSheetFrameDataRaw(uint16_t& spriteWidth, uint16_t& spriteHeight, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	spriteWidth = 0;
	spriteHeight = 0;

	auto shtResource = this->getResource(Resource::TYPE_PARTICLE_SHEET_DATA);
	if (!shtResource) {
		return {};
	}

	auto sht = shtResource->getDataAsParticleSheet();
	const auto* sequence = sht.getSequenceFromID(shtSequenceID);
	if (!sequence || sequence->frames.size() <= shtFrame || shtBounds >= sht.getFrameBoundsCount()) {
		return {};
	}

	// These values are likely slightly too large thanks to float magic, use a
	// shader that scales UVs instead of this function if precision is a concern
	// This will also break if any of the bounds are above 1 or below 0, but that
	// hasn't been observed in official textures
	const auto& bounds = sequence->frames[shtFrame].bounds[shtBounds];
	uint16_t x1 = std::clamp<uint16_t>(std::floor(bounds.x1 * static_cast<float>(this->getWidth(mip))),  0, this->getWidth(mip));
	uint16_t y1 = std::clamp<uint16_t>(std::ceil( bounds.y1 * static_cast<float>(this->getHeight(mip))), 0, this->getHeight(mip));
	uint16_t x2 = std::clamp<uint16_t>(std::ceil( bounds.x2 * static_cast<float>(this->getWidth(mip))),  0, this->getHeight(mip));
	uint16_t y2 = std::clamp<uint16_t>(std::floor(bounds.y2 * static_cast<float>(this->getHeight(mip))), 0, this->getWidth(mip));

	if (x1 > x2) [[unlikely]] {
		std::swap(x1, x2);
	}
	if (y1 > y2) [[unlikely]] {
		std::swap(y1, y2);
	}
	spriteWidth = x2 - x1;
	spriteWidth = y2 - y1;

	const auto out = ImageConversion::cropImageData(this->getImageDataRaw(mip, frame, face, slice), this->getFormat(), this->getWidth(mip), spriteWidth, x1, this->getHeight(mip), spriteHeight, y1);
	if (out.empty()) {
		spriteWidth = 0;
		spriteHeight = 0;
	}
	return out;
}

std::vector<std::byte> VTF::getParticleSheetFrameDataAs(ImageFormat newFormat, uint16_t& spriteWidth, uint16_t& spriteHeight, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	return ImageConversion::convertImageDataToFormat(this->getParticleSheetFrameDataRaw(spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice), this->getFormat(), newFormat, spriteWidth, spriteHeight);
}

std::vector<std::byte> VTF::getParticleSheetFrameDataAsRGBA8888(uint16_t& spriteWidth, uint16_t& spriteHeight, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	return this->getParticleSheetFrameDataAs(ImageFormat::RGBA8888, spriteWidth, spriteHeight, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice);
}

void VTF::setParticleSheetResource(const SHT& value) {
	std::vector<std::byte> particleSheetData;
	BufferStream writer{particleSheetData};

	const auto bakedSheet = value.bake();
	writer.write<uint32_t>(bakedSheet.size()).write(bakedSheet);
	particleSheetData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_PARTICLE_SHEET_DATA, particleSheetData);
}

void VTF::removeParticleSheetResource() {
	this->removeResourceInternal(Resource::TYPE_PARTICLE_SHEET_DATA);
}

void VTF::setCRCResource(uint32_t value) {
	this->setResourceInternal(Resource::TYPE_CRC, {reinterpret_cast<const std::byte*>(&value), sizeof(value)});
}

void VTF::removeCRCResource() {
	this->removeResourceInternal(Resource::TYPE_CRC);
}

void VTF::setLODResource(uint8_t u, uint8_t v, uint8_t u360, uint8_t v360) {
	uint32_t lodData;
	BufferStream writer{&lodData, sizeof(lodData)};

	writer << u << v << u360 << v360;

	this->setResourceInternal(Resource::TYPE_LOD_CONTROL_INFO, {reinterpret_cast<const std::byte*>(&lodData), sizeof(lodData)});
}

void VTF::removeLODResource() {
	this->removeResourceInternal(Resource::TYPE_LOD_CONTROL_INFO);
}

void VTF::setExtendedFlagsResource(uint32_t value) {
	this->setResourceInternal(Resource::TYPE_EXTENDED_FLAGS, {reinterpret_cast<const std::byte*>(&value), sizeof(value)});
}

void VTF::removeExtendedFlagsResource() {
	this->removeResourceInternal(Resource::TYPE_EXTENDED_FLAGS);
}

void VTF::setKeyValuesDataResource(const std::string& value) {
	std::vector<std::byte> keyValuesData;
	BufferStream writer{keyValuesData};

	writer.write<uint32_t>(value.size()).write(value, false);
	keyValuesData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_KEYVALUES_DATA, keyValuesData);
}

void VTF::removeKeyValuesDataResource() {
	this->removeResourceInternal(Resource::TYPE_KEYVALUES_DATA);
}

void VTF::setHotspotDataResource(const HOT& value) {
	std::vector<std::byte> hotspotData;
	BufferStream writer{hotspotData};

	const auto bakedHotspotData = value.bake();
	writer.write<uint32_t>(bakedHotspotData.size()).write(bakedHotspotData);
	hotspotData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_HOTSPOT_DATA, hotspotData);
}

void VTF::removeHotspotDataResource() {
	this->removeResourceInternal(Resource::TYPE_HOTSPOT_DATA);
}

int16_t VTF::getCompressionLevel() const {
	return this->compressionLevel;
}

void VTF::setCompressionLevel(int16_t newCompressionLevel) {
	this->compressionLevel = newCompressionLevel;
}

CompressionMethod VTF::getCompressionMethod() const {
	return this->compressionMethod;
}

void VTF::setCompressionMethod(CompressionMethod newCompressionMethod) {
	if (newCompressionMethod == CompressionMethod::CONSOLE_LZMA && this->platform == VTF::PLATFORM_PC) {
		this->compressionMethod = CompressionMethod::ZSTD;
	} else if (newCompressionMethod != CompressionMethod::CONSOLE_LZMA && this->platform != VTF::PLATFORM_PC) {
		this->compressionMethod = CompressionMethod::CONSOLE_LZMA;
	} else {
		this->compressionMethod = newCompressionMethod;
	}
}

bool VTF::hasImageData() const {
	return this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0;
}

std::span<const std::byte> VTF::getImageDataRaw(uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	if (const auto imageResource = this->getResource(Resource::TYPE_IMAGE_DATA)) {
		if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, frame, this->frameCount, face, this->getFaceCount(), this->width, this->height, slice, this->sliceCount)) {
			return imageResource->data.subspan(offset, length);
		}
	}
	return {};
}

std::vector<std::byte> VTF::getImageDataAs(ImageFormat newFormat, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	const auto rawImageData = this->getImageDataRaw(mip, frame, face, slice);
	if (rawImageData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFormat(rawImageData, this->format, newFormat, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
}

std::vector<std::byte> VTF::getImageDataAsRGBA8888(uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	return this->getImageDataAs(ImageFormat::RGBA8888, mip, frame, face, slice);
}

bool VTF::setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality) {
	if (imageData_.empty()) {
		return false;
	}

	if (!this->hasImageData()) {
		uint16_t resizedWidth = width_, resizedHeight = height_;
		ImageConversion::setResizedDims(resizedWidth, this->imageWidthResizeMethod, resizedHeight, this->imageHeightResizeMethod);
		if (ImageFormatDetails::compressed(format_)) {
			resizedWidth += math::paddingForAlignment(4, resizedWidth);
			resizedHeight += math::paddingForAlignment(4, resizedHeight);
		}
		if (const auto newMipCount = ImageDimensions::getRecommendedMipCountForDims(format_, resizedWidth, resizedHeight); newMipCount <= mip) {
			mip = newMipCount - 1;
		}
		if (face > 6 || (face == 6 && (this->version < 1 || this->version > 4))) {
			return false;
		}
		this->regenerateImageData(format_, resizedWidth, resizedHeight, mip + 1, frame + 1, face ? (face < 5 ? 5 : face) : 0, slice + 1);
	}

	const auto faceCount = this->getFaceCount();
	if (this->mipCount <= mip || this->frameCount <= frame || faceCount <= face || this->sliceCount <= slice) {
		return false;
	}

	const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA);
	if (!imageResource) {
		return false;
	}
	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, frame, this->frameCount, face, faceCount, this->width, this->height, slice, this->sliceCount)) {
		std::vector<std::byte> image{imageData_.begin(), imageData_.end()};
		const auto newWidth = ImageDimensions::getMipDim(mip, this->width);
		const auto newHeight = ImageDimensions::getMipDim(mip, this->height);
		if (width_ != newWidth || height_ != newHeight) {
			image = ImageConversion::resizeImageData(image, format_, width_, newWidth, height_, newHeight, this->isSRGB(), filter);
		}
		if (format_ != this->format) {
			image = ImageConversion::convertImageDataToFormat(image, format_, this->format, newWidth, newHeight, quality);
		}
		std::memcpy(imageResource->data.data() + offset, image.data(), image.size());
	}
	return true;
}

bool VTF::setImage(const std::string& imagePath, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality) {
	ImageFormat inputFormat;
	int inputWidth, inputHeight, inputFrameCount;
	auto imageData_ = ImageConversion::convertFileToImageData(fs::readFileBuffer(imagePath), inputFormat, inputWidth, inputHeight, inputFrameCount);

	// Unable to decode file
	if (imageData_.empty() || inputFormat == ImageFormat::EMPTY || !inputWidth || !inputHeight || !inputFrameCount) {
		return false;
	}

	// One frame (normal)
	if (inputFrameCount == 1) {
		return this->setImage(imageData_, inputFormat, inputWidth, inputHeight, filter, mip, frame, face, slice, quality);
	}

	// Multiple frames (GIF)
	bool allSuccess = true;
	const auto frameSize = ImageFormatDetails::getDataLength(inputFormat, inputWidth, inputHeight);
	for (int currentFrame = 0; currentFrame < inputFrameCount; currentFrame++) {
		if (!this->setImage({imageData_.data() + currentFrame * frameSize, imageData_.data() + currentFrame * frameSize + frameSize}, inputFormat, inputWidth, inputHeight, filter, mip, frame + currentFrame, face, slice, quality)) {
			allSuccess = false;
		}
		if (currentFrame == 0 && this->frameCount < frame + inputFrameCount) {
			// Call this after setting the first image, this function is a no-op if no image data is present yet
			this->setFrameCount(frame + inputFrameCount);
		}
	}
	return allSuccess;
}

std::vector<std::byte> VTF::saveImageToFile(uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, ImageConversion::FileFormat fileFormat) const {
	return ImageConversion::convertImageDataToFile(this->getImageDataRaw(mip, frame, face, slice), this->format, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height), fileFormat);
}

bool VTF::saveImageToFile(const std::string& imagePath, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, ImageConversion::FileFormat fileFormat) const {
	if (auto data_ = this->saveImageToFile(mip, frame, face, slice, fileFormat); !data_.empty()) {
		return fs::writeFileBuffer(imagePath, data_);
	}
	return false;
}

bool VTF::hasThumbnailData() const {
	return this->thumbnailFormat != ImageFormat::EMPTY && this->thumbnailWidth > 0 && this->thumbnailHeight > 0;
}

std::span<const std::byte> VTF::getThumbnailDataRaw() const {
	if (const auto thumbnailResource = this->getResource(Resource::TYPE_THUMBNAIL_DATA)) {
		return thumbnailResource->data;
	}
	return {};
}

std::vector<std::byte> VTF::getThumbnailDataAs(ImageFormat newFormat) const {
	const auto rawThumbnailData = this->getThumbnailDataRaw();
	if (rawThumbnailData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFormat(rawThumbnailData, this->thumbnailFormat, newFormat, this->thumbnailWidth, this->thumbnailHeight);
}

std::vector<std::byte> VTF::getThumbnailDataAsRGBA8888() const {
	return this->getThumbnailDataAs(ImageFormat::RGBA8888);
}

void VTF::setThumbnail(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, float quality) {
	if (format_ != this->thumbnailFormat) {
		this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, ImageConversion::convertImageDataToFormat(imageData_, format_, this->thumbnailFormat, width_, height_, quality));
	} else {
		this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, imageData_);
	}
	this->thumbnailWidth = width_;
	this->thumbnailHeight = height_;
}

bool VTF::setThumbnail(const std::string& imagePath, float quality) {
	ImageFormat inputFormat;
	int inputWidth, inputHeight, inputFrameCount;
	auto imageData_ = ImageConversion::convertFileToImageData(fs::readFileBuffer(imagePath), inputFormat, inputWidth, inputHeight, inputFrameCount);

	// Unable to decode file
	if (imageData_.empty() || inputFormat == ImageFormat::EMPTY || !inputWidth || !inputHeight || !inputFrameCount) {
		return false;
	}

	// One frame (normal)
	if (inputFrameCount == 1) {
		this->setThumbnail(imageData_, inputFormat, inputWidth, inputHeight, quality);
		return true;
	}

	// Multiple frames (GIF) - we will just use the first one
	const auto frameSize = ImageFormatDetails::getDataLength(inputFormat, inputWidth, inputHeight);
	this->setThumbnail({imageData_.data(), frameSize}, inputFormat, inputWidth, inputHeight, quality);
	return true;
}

void VTF::computeThumbnail(ImageConversion::ResizeFilter filter, float quality) {
	if (!this->hasImageData()) {
		return;
	}
	this->thumbnailFormat = ImageFormat::DXT1;
	this->thumbnailWidth = 16;
	this->thumbnailHeight = 16;
	this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, ImageConversion::convertImageDataToFormat(ImageConversion::resizeImageData(this->getImageDataRaw(), this->format, this->width, this->thumbnailWidth, this->height, this->thumbnailHeight, this->isSRGB(), filter), this->format, this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight, quality));
}

void VTF::removeThumbnail() {
	this->thumbnailFormat = ImageFormat::EMPTY;
	this->thumbnailWidth = 0;
	this->thumbnailHeight = 0;
	this->removeResourceInternal(Resource::TYPE_THUMBNAIL_DATA);
}

std::vector<std::byte> VTF::saveThumbnailToFile(ImageConversion::FileFormat fileFormat) const {
	return ImageConversion::convertImageDataToFile(this->getThumbnailDataRaw(), this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight, fileFormat);
}

bool VTF::saveThumbnailToFile(const std::string& imagePath, ImageConversion::FileFormat fileFormat) const {
	if (auto data_ = this->saveThumbnailToFile(fileFormat); !data_.empty()) {
		return fs::writeFileBuffer(imagePath, data_);
	}
	return false;
}

bool VTF::hasFallbackData() const {
	return this->fallbackWidth > 0 && this->fallbackHeight > 0 && this->fallbackMipCount > 0;
}

std::span<const std::byte> VTF::getFallbackDataRaw(uint8_t mip, uint16_t frame, uint8_t face) const {
	if (const auto fallbackResource = this->getResource(Resource::TYPE_FALLBACK_DATA)) {
		if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->fallbackMipCount, frame, this->frameCount, face, this->getFaceCount(), this->fallbackWidth, this->fallbackHeight)) {
			return fallbackResource->data.subspan(offset, length);
		}
	}
	return {};
}

std::vector<std::byte> VTF::getFallbackDataAs(ImageFormat newFormat, uint8_t mip, uint16_t frame, uint8_t face) const {
	const auto rawFallbackData = this->getFallbackDataRaw(mip, frame, face);
	if (rawFallbackData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFormat(rawFallbackData, this->format, newFormat, this->fallbackWidth, this->fallbackHeight);
}

std::vector<std::byte> VTF::getFallbackDataAsRGBA8888(uint8_t mip, uint16_t frame, uint8_t face) const {
	return this->getFallbackDataAs(ImageFormat::RGBA8888, mip, frame, face);
}

void VTF::computeFallback(ImageConversion::ResizeFilter filter) {
	if (!this->hasImageData()) {
		return;
	}

	const auto* imageResource = this->getResourceInternal(Resource::TYPE_IMAGE_DATA);
	if (!imageResource) {
		return;
	}

	const auto faceCount = this->getFaceCount();

	this->fallbackWidth = 8;
	this->fallbackHeight = 8;
	this->fallbackMipCount = ImageDimensions::getActualMipCountForDimsOnConsole(this->fallbackWidth, this->fallbackHeight);

	std::vector<std::byte> fallbackData;
	fallbackData.resize(ImageFormatDetails::getDataLength(this->format, this->fallbackMipCount, this->frameCount, faceCount, this->fallbackWidth, this->fallbackHeight));
	for (int i = this->fallbackMipCount - 1; i >= 0; i--) {
		for (int j = 0; j < this->frameCount; j++) {
			for (int k = 0; k < faceCount; k++) {
				auto mip = ImageConversion::resizeImageData(this->getImageDataRaw(0, j, k, 0), this->format, this->width, ImageDimensions::getMipDim(i, this->fallbackWidth), this->height, ImageDimensions::getMipDim(i, this->fallbackHeight), this->isSRGB(), filter);
				if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, i, this->fallbackMipCount, j, this->frameCount, k, faceCount, this->fallbackWidth, this->fallbackHeight) && mip.size() == length) {
					std::memcpy(fallbackData.data() + offset, mip.data(), length);
				}
			}
		}
	}
	this->setResourceInternal(Resource::TYPE_FALLBACK_DATA, fallbackData);
}

void VTF::removeFallback() {
	this->fallbackWidth = 0;
	this->fallbackHeight = 0;
	this->fallbackMipCount = 0;
	this->removeResourceInternal(Resource::TYPE_FALLBACK_DATA);
}

std::vector<std::byte> VTF::saveFallbackToFile(uint8_t mip, uint16_t frame, uint8_t face, ImageConversion::FileFormat fileFormat) const {
	return ImageConversion::convertImageDataToFile(this->getFallbackDataRaw(mip, frame, face), this->format, ImageDimensions::getMipDim(mip, this->fallbackWidth), ImageDimensions::getMipDim(mip, this->fallbackHeight), fileFormat);
}

bool VTF::saveFallbackToFile(const std::string& imagePath, uint8_t mip, uint16_t frame, uint8_t face, ImageConversion::FileFormat fileFormat) const {
	if (auto data_ = this->saveFallbackToFile(mip, frame, face, fileFormat); !data_.empty()) {
		return fs::writeFileBuffer(imagePath, data_);
	}
	return false;
}

uint8_t VTF::getXBOXMipScale() const {
	return this->xboxMipScale;
}

void VTF::setXBOXMipScale(uint8_t xboxMipScale_) {
	this->xboxMipScale = xboxMipScale_;
}

std::vector<std::byte> VTF::bake() const {
	std::vector<std::byte> out;
	BufferStream writer{out};

	static constexpr auto writeNonLocalResource = [](BufferStream& writer_, Resource::Type type, std::span<const std::byte> data, VTF::Platform platform) {
		if (platform != VTF::PLATFORM_PC) {
			BufferStream::swap_endian(reinterpret_cast<uint32_t*>(&type));
		}
		writer_.write<uint32_t>(type);
		const auto resourceOffsetPos = writer_.tell();
		writer_.seek(0, std::ios::end);
		const auto resourceOffsetValue = writer_.tell();
		writer_.write(data);
		writer_.seek_u(resourceOffsetPos).write<uint32_t>(resourceOffsetValue);
	};

	// HACK: no source game supports this format, but they do support the flag with reg. DXT1
	auto bakeFormat = this->format;
	auto bakeFlags = this->flags;
	if (bakeFormat == ImageFormat::DXT1_ONE_BIT_ALPHA) {
		bakeFormat = ImageFormat::DXT1;
		bakeFlags |= FLAG_ONE_BIT_ALPHA;
	}
	// HACK: NV_NULL / ATI2N / ATI1N are at a different position based on engine branch
	if (this->version < 5 && (this->format == ImageFormat::EMPTY || this->format == ImageFormat::ATI2N || this->format == ImageFormat::ATI1N)) {
		bakeFormat = static_cast<ImageFormat>(static_cast<int32_t>(this->format) + 3);
	}

	switch (this->platform) {
		case PLATFORM_UNKNOWN:
			return out;
		case PLATFORM_PC: {
			writer
				.write(VTF_SIGNATURE)
				.write<int32_t>(7)
				.write(this->version);

			const auto headerLengthPos = writer.tell();
			writer.write<uint32_t>(0);

			writer
				.write(this->width)
				.write(this->height)
				.write(bakeFlags)
				.write(this->frameCount)
				.write(this->startFrame)
				.pad<uint32_t>()
				.write(this->reflectivity)
				.pad<uint32_t>()
				.write(this->bumpMapScale)
				.write(bakeFormat)
				.write(this->mipCount)
				.write(ImageFormat::DXT1)
				.write(this->thumbnailWidth)
				.write(this->thumbnailHeight);

			if (this->version >= 2) {
				writer << this->sliceCount;
			}

			if (this->version < 3) {
				writer.pad(math::paddingForAlignment(16, writer.tell()));
				const auto headerSize = writer.tell();
				writer.seek_u(headerLengthPos).write<uint32_t>(headerSize).seek_u(headerSize);

				if (const auto* thumbnailResource = this->getResource(Resource::TYPE_THUMBNAIL_DATA); thumbnailResource && this->hasThumbnailData()) {
					writer.write(thumbnailResource->data);
				}
				if (const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA); imageResource && this->hasImageData()) {
					writer.write(imageResource->data);
				}
			} else {
				std::vector<std::byte> auxCompressionResourceData;
				std::vector<std::byte> compressedImageResourceData;
				bool hasAuxCompression = false;
				if (const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA)) {
					hasAuxCompression = this->version >= 6 && this->compressionLevel != 0;
					if (hasAuxCompression) {
						const auto faceCount = this->getFaceCount();
						auxCompressionResourceData.resize((this->mipCount * this->frameCount * faceCount + 2) * sizeof(uint32_t));
						BufferStream auxWriter{auxCompressionResourceData, false};

						// Format of aux resource is as follows, with each item of unspecified type being a 4 byte integer:
						// - Size of resource in bytes, not counting this int
						// - Compression level, method (2 byte integers)
						// - (X times) Size of each mip-face-frame combo
						auxWriter
							.write<uint32_t>(auxCompressionResourceData.size() - sizeof(uint32_t))
							.write(this->compressionLevel)
							.write(this->compressionMethod);

						for (int i = this->mipCount - 1; i >= 0; i--) {
							for (int j = 0; j < this->frameCount; j++) {
								for (int k = 0; k < faceCount; k++) {
									if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, i, this->mipCount, j, this->frameCount, k, faceCount, this->width, this->height, 0, this->sliceCount)) {
										auto compressedData = ::compressData({imageResource->data.data() + offset, length * this->sliceCount}, this->compressionLevel, this->compressionMethod);
										compressedImageResourceData.insert(compressedImageResourceData.end(), compressedData.begin(), compressedData.end());
										auxWriter.write<uint32_t>(compressedData.size());
									}
								}
							}
						}
					}
				}

				writer.pad(3).write<uint32_t>(this->getResources().size() + hasAuxCompression).pad(8);

				const auto resourceStart = writer.tell();
				const auto headerSize = resourceStart + ((this->getResources().size() + hasAuxCompression) * sizeof(uint64_t));
				writer.seek_u(headerLengthPos).write<uint32_t>(headerSize).seek_u(resourceStart);
				while (writer.tell() < headerSize) {
					writer.write<uint64_t>(0);
				}
				writer.seek_u(resourceStart);

				for (const auto resourceType : Resource::getOrder()) {
					if (hasAuxCompression && resourceType == Resource::TYPE_AUX_COMPRESSION) {
						writeNonLocalResource(writer, resourceType, auxCompressionResourceData, this->platform);
					} else if (hasAuxCompression && resourceType == Resource::TYPE_IMAGE_DATA) {
						writeNonLocalResource(writer, resourceType, compressedImageResourceData, this->platform);
					} else if (const auto* resource = this->getResource(resourceType)) {
						if ((resource->flags & Resource::FLAG_LOCAL_DATA) && resource->data.size() == sizeof(uint32_t)) {
							writer.write<uint32_t>((Resource::FLAG_LOCAL_DATA << 24) | resource->type);
							writer.write(resource->data);
						} else {
							writeNonLocalResource(writer, resource->type, resource->data, this->platform);
						}
					}
				}
			}
			break;
		}
		case PLATFORM_XBOX: {
			writer << XTF_SIGNATURE << PLATFORM_XBOX;
			writer.write<uint32_t>(0);

			const auto headerSizePos = writer.tell();
			writer
				.write<uint32_t>(0)
				.write(this->flags)
				.write(this->width)
				.write(this->height)
				.write(this->sliceCount)
				.write(this->frameCount);
			const auto preloadSizePos = writer.tell();
			writer.write<uint16_t>(0);
			const auto imageOffsetPos = writer.tell();
			writer
				.write<uint16_t>(0)
				.write(this->reflectivity[0])
				.write(this->reflectivity[1])
				.write(this->reflectivity[2])
				.write(this->bumpMapScale)
				.write(this->format)
				.write(this->thumbnailWidth)
				.write(this->thumbnailHeight)
				.write(this->fallbackWidth)
				.write(this->fallbackHeight)
				.write(this->xboxMipScale)
				.write<uint8_t>(0);

			const auto headerSize = writer.tell();
			writer.seek_u(headerSizePos).write<uint32_t>(headerSize).seek_u(headerSize);

			if (const auto* thumbnailResource = this->getResource(Resource::TYPE_THUMBNAIL_DATA); thumbnailResource && this->hasThumbnailData()) {
				writer.write(thumbnailResource->data);
			}

			if (const auto* fallbackResource = this->getResource(Resource::TYPE_FALLBACK_DATA); fallbackResource && this->hasFallbackData()) {
				bool ok;
				auto reorderedFallbackData = ::convertBetweenDDSAndVTFMipOrderForXBOX<false>(false, fallbackResource->data, this->format, this->fallbackMipCount, this->frameCount, this->getFaceCount(), this->fallbackWidth, this->fallbackHeight, 1, ok);
				if (ok) {
					::swapImageDataEndianForConsole<false>(reorderedFallbackData, this->format, this->fallbackMipCount, this->frameCount, this->getFaceCount(), this->fallbackWidth, this->fallbackHeight, 1, this->platform);
					writer.write(reorderedFallbackData);
				} else {
					writer.pad(fallbackResource->data.size());
				}
			}

			const auto preloadSize = writer.tell();
			writer.seek_u(preloadSizePos).write<uint32_t>(preloadSize).seek_u(preloadSize);

			writer.pad(math::paddingForAlignment(512, writer.tell()));
			const auto imageOffset = writer.tell();
			writer.seek_u(imageOffsetPos).write<uint16_t>(imageOffset).seek_u(imageOffset);

			if (const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA); imageResource && this->hasImageData()) {
				bool ok;
				auto reorderedImageData = ::convertBetweenDDSAndVTFMipOrderForXBOX<false>(true, imageResource->data, this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount, ok);
				if (ok) {
					::swapImageDataEndianForConsole<false>(reorderedImageData, this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount, this->platform);
					writer.write(reorderedImageData);
				} else {
					writer.pad(imageResource->data.size());
				}
			}
			writer.pad(math::paddingForAlignment(512, writer.tell()));
			break;
		}
		case PLATFORM_X360:
		case PLATFORM_PS3_ORANGEBOX:
		case PLATFORM_PS3_PORTAL2: {
			if (this->platform == PLATFORM_PS3_PORTAL2) {
				writer << VTF3_SIGNATURE;
				writer.set_big_endian(true);
				writer << PLATFORM_PS3_ORANGEBOX; // Intentional
			} else {
				writer << VTFX_SIGNATURE;
				writer.set_big_endian(true);
				writer << this->platform;
			}
			writer.write<uint32_t>(8);

			// Go down until top level texture is <1mb, matches makegamedata.exe output
			uint8_t mipSkip = 0;
			for (int mip = 0; mip < this->mipCount; mip++, mipSkip++) {
				if (ImageFormatDetails::getDataLength(this->format, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height), ImageDimensions::getMipDim(mip, this->sliceCount)) < 1024 * 1024) {
					break;
				}
			}

			const auto headerLengthPos = writer.tell();
			writer
				.write<uint32_t>(0)
				.write(bakeFlags)
				.write(this->width)
				.write(this->height)
				.write(this->sliceCount)
				.write(this->frameCount);
			const auto preloadPos = writer.tell();
			writer
				.write<uint16_t>(0) // preload size
				.write(mipSkip)
				.write<uint8_t>(this->resources.size())
				.write(this->reflectivity[0])
				.write(this->reflectivity[1])
				.write(this->reflectivity[2])
				.write(this->bumpMapScale)
				.write(bakeFormat)
				.write<uint8_t>(std::clamp(static_cast<int>(std::roundf(this->reflectivity[0] * 255)), 0, 255))
				.write<uint8_t>(std::clamp(static_cast<int>(std::roundf(this->reflectivity[1] * 255)), 0, 255))
				.write<uint8_t>(std::clamp(static_cast<int>(std::roundf(this->reflectivity[2] * 255)), 0, 255))
				.write<uint8_t>(255);
			const auto compressionPos = writer.tell();
			writer.write<uint32_t>(0); // compressed length

			if (this->platform == PLATFORM_PS3_PORTAL2) {
				// 16 byte aligned
				writer.write<uint32_t>(0);
			}

			// LZMA compression has not been observed on the PS3 copy of The Orange Box
			// todo(vtfpp): check cubemaps
			std::vector<std::byte> imageResourceData;
			bool hasCompression = false;
			if (const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA)) {
				imageResourceData.assign(imageResource->data.begin(), imageResource->data.end());
				::swapImageDataEndianForConsole<false>(imageResourceData, this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount, this->platform);

				if (this->platform != PLATFORM_PS3_ORANGEBOX) {
					hasCompression = this->compressionMethod == CompressionMethod::CONSOLE_LZMA;
					if (hasCompression) {
						auto fixedCompressionLevel = this->compressionLevel;
						if (this->compressionLevel == 0) {
							// Compression level defaults to 0, so it works differently on console.
							// Rather than not compress on 0, 0 will be replaced with the default
							// compression level (6) if the compression method is LZMA.
							fixedCompressionLevel = 6;
						}
						if (const auto compressedData = compression::compressValveLZMA(imageResourceData, fixedCompressionLevel)) {
							imageResourceData.assign(compressedData->begin(), compressedData->end());
						} else {
							hasCompression = false;
						}
					}
				}
			}

			const auto resourceStart = writer.tell();
			const auto headerSize = resourceStart + (this->getResources().size() * sizeof(uint64_t));
			writer.seek_u(headerLengthPos).write<uint32_t>(headerSize).seek_u(resourceStart);
			while (writer.tell() < headerSize) {
				writer.write<uint64_t>(0);
			}
			writer.seek_u(resourceStart);

			for (const auto resourceType : Resource::getOrder()) {
				if (resourceType == Resource::TYPE_IMAGE_DATA) {
					auto curPos = writer.tell();
					const auto imagePos = writer.seek(0, std::ios::end).tell();
					writer.seek_u(preloadPos).write(std::max<uint16_t>(imagePos, 2048)).seek_u(curPos);

					writeNonLocalResource(writer, resourceType, imageResourceData, this->platform);

					if (hasCompression) {
						curPos = writer.tell();
						writer.seek_u(compressionPos).write<uint32_t>(imageResourceData.size()).seek_u(curPos);
					}
				} else if (const auto* resource = this->getResource(resourceType)) {
					std::vector<std::byte> resData{resource->data.begin(), resource->data.end()};

					if (resource->type == Resource::TYPE_THUMBNAIL_DATA) {
						::swapImageDataEndianForConsole<false>(resData, this->thumbnailFormat, 1, 1, 1, this->thumbnailWidth, this->thumbnailHeight, 1, this->platform);
					} else if (!(resource->flags & Resource::FLAG_LOCAL_DATA) && resData.size() >= sizeof(uint32_t)) {
						BufferStream::swap_endian(reinterpret_cast<uint32_t*>(resData.data()));
					}

					if ((resource->flags & Resource::FLAG_LOCAL_DATA) && resource->data.size() == sizeof(uint32_t)) {
						writer.set_big_endian(false);
						writer.write<uint32_t>((Resource::FLAG_LOCAL_DATA << 24) | resource->type);
						writer.set_big_endian(true);
						writer.write(resData);
					} else {
						writeNonLocalResource(writer, resource->type, resData, this->platform);
					}
				}
			}
			break;
		}
	}
	out.resize(writer.size());
	return out;
}

bool VTF::bake(const std::string& vtfPath) const {
	return fs::writeFileBuffer(vtfPath, this->bake());
}
