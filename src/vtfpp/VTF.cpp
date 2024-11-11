#include <vtfpp/VTF.h>

#include <algorithm>
#include <cstring>
#include <unordered_map>

#ifdef SOURCEPP_BUILD_WITH_THREADS
#include <future>
#include <thread>
#endif

#include <BufferStream.h>
#include <miniz.h>
#include <zstd.h>

#include <vtfpp/ImageConversion.h>

using namespace sourcepp;
using namespace vtfpp;

namespace {

std::vector<std::byte> compressData(std::span<const std::byte> data, int16_t level, CompressionMethod method) {
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

			auto expectedSize = ZSTD_compressBound(data.size());
			std::vector<std::byte> out(expectedSize);

			auto compressedSize = ZSTD_compress(out.data(), expectedSize, data.data(), data.size(), level);
			if (ZSTD_isError(compressedSize)) {
				return {};
			}

			out.resize(compressedSize);
			return out;
		}
	}
	return {};
}

} // namespace

const std::array<Resource::Type, 8>& Resource::getOrder() {
	static constinit std::array<Type, 8> typeArray{
		TYPE_THUMBNAIL_DATA,
		TYPE_IMAGE_DATA,
		TYPE_PARTICLE_SHEET_DATA,
		TYPE_CRC,
		TYPE_LOD_CONTROL_INFO,
		TYPE_EXTENDED_FLAGS,
		TYPE_KEYVALUES_DATA,
		TYPE_AUX_COMPRESSION,
	};
	static bool unsorted = true;
	if (unsorted) {
		std::sort(typeArray.begin(), typeArray.end());
		unsorted = false;
	}
	return typeArray;
}

Resource::ConvertedData Resource::convertData() const {
	switch (this->type) {
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
			return std::make_pair(
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 0),
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 1));
		case TYPE_KEYVALUES_DATA:
			if (this->data.size() <= sizeof(uint32_t)) {
				return "";
			}
			return std::string(reinterpret_cast<const char*>(this->data.data()) + 4, *reinterpret_cast<const uint32_t*>(this->data.data()));
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
	this->majorVersion = 7;
	this->minorVersion = 4;

	this->flags |= FLAG_NO_MIP | FLAG_NO_LOD;

	this->format = ImageFormat::EMPTY;
	this->thumbnailFormat = ImageFormat::EMPTY;

	this->width = 0;
	this->height = 0;
	this->mipCount = 0;
	this->frameCount = 0;
	this->sliceCount = 0;

	this->opened = true;
}

VTF::VTF(std::vector<std::byte>&& vtfData, bool parseHeaderOnly)
		: data(std::move(vtfData)) {
	BufferStreamReadOnly stream{this->data};

	if (stream.read<uint32_t>() != VTF_SIGNATURE) {
		return;
	}

	stream >> this->majorVersion >> this->minorVersion;
	if (this->majorVersion != 7 || this->minorVersion > 6) {
		return;
	}
	const auto headerSize = stream.read<uint32_t>();

	stream
		.read(this->width)
		.read(this->height)
		.read(this->flags)
		.read(this->frameCount)
		.read(this->startFrame)
		.skip(4)
		.read(this->reflectivity)
		.skip(4)
		.read(this->bumpMapScale)
		.read(this->format)
		.read(this->mipCount);

	// This will always be DXT1
	stream.skip<ImageFormat>();
	stream >> this->thumbnailWidth >> this->thumbnailHeight;
	if (this->thumbnailWidth == 0 || this->thumbnailHeight == 0) {
		this->thumbnailFormat = ImageFormat::EMPTY;
	} else {
		this->thumbnailFormat = ImageFormat::DXT1;
	}

	if (this->minorVersion < 2) {
		this->sliceCount = 1;
	} else {
		stream.read(this->sliceCount);
	}

	if (parseHeaderOnly) {
		this->opened = true;
		return;
	}

	if (this->minorVersion >= 3) {
		stream.skip(3);
		auto resourceCount = stream.read<uint32_t>();
		stream.skip(8);

		if (resourceCount > VTF::MAX_RESOURCES) {
			resourceCount = VTF::MAX_RESOURCES;
		}
		this->resources.reserve(resourceCount);

		Resource* lastResource = nullptr;
		for (int i = 0; i < resourceCount; i++) {
			auto& [type, flags_, data_] = this->resources.emplace_back();

			auto typeAndFlags = stream.read<uint32_t>();
			type = static_cast<Resource::Type>(typeAndFlags & 0xffffff); // last 3 bytes
			flags_ = static_cast<Resource::Flags>(typeAndFlags >> 24); // first byte
			data_ = stream.read_span<std::byte>(4);

			if (!(flags_ & Resource::FLAG_LOCAL_DATA)) {
				if (lastResource) {
					auto lastOffset = *reinterpret_cast<uint32_t*>(lastResource->data.data());
					auto currentOffset = *reinterpret_cast<uint32_t*>(data_.data());

					auto curPos = stream.tell();
					stream.seek(lastOffset);
					lastResource->data = stream.read_span<std::byte>(currentOffset - lastOffset);
					stream.seek(static_cast<int64_t>(curPos));
				}
				lastResource = &this->resources.back();
			}
		}
		if (lastResource) {
			auto offset = *reinterpret_cast<uint32_t*>(lastResource->data.data());
			auto curPos = stream.tell();
			stream.seek(offset);
			lastResource->data = stream.read_span<std::byte>(stream.size() - offset);
			stream.seek(static_cast<int64_t>(curPos));
		}

		this->opened = stream.tell() == headerSize;

		if (this->opened && this->minorVersion >= 6) {
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
	this->majorVersion = other.majorVersion;
	this->minorVersion = other.minorVersion;
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
	this->sliceCount = other.sliceCount;

	this->resources.clear();
	for (const auto& [otherType, otherFlags, otherData] : other.resources) {
		auto& [type, flags_, data_] = this->resources.emplace_back();
		type = otherType;
		flags_ = otherFlags;
		data_ = {this->data.data() + (otherData.data() - other.data.data()), otherData.size()};
	}

	this->compressionLevel = other.compressionLevel;
	this->compressionMethod = other.compressionMethod;
	this->imageWidthResizeMethod = other.imageWidthResizeMethod;
	this->imageHeightResizeMethod = other.imageHeightResizeMethod;

	return *this;
}

VTF::operator bool() const {
	return this->opened;
}

ImageFormat VTF::getDefaultFormat() const {
	if (this->format != ImageFormat::EMPTY) {
		if (ImageFormatDetails::decompressedAlpha(this->format) > 0) {
			if (this->majorVersion >= 7 && this->minorVersion >= 6) {
				return ImageFormat::BC7;
			}
			return ImageFormat::DXT5;
		}
	}
	return ImageFormat::DXT1;
}

void VTF::createInternal(VTF& writer, CreationOptions options) {
	if (options.initialFrameCount > 1 || options.isCubeMap || options.initialSliceCount > 1) {
		writer.setFrameFaceAndSliceCount(options.initialFrameCount, options.isCubeMap, options.hasSphereMap, options.initialSliceCount);
	}
	writer.setStartFrame(options.startFrame);
	writer.setBumpMapScale(options.bumpMapScale);
	if (options.createReflectivity) {
		writer.computeReflectivity();
	}
	if (options.createThumbnail) {
		writer.computeThumbnail();
	}
	if (options.outputFormat == VTF::FORMAT_UNCHANGED) {
		options.outputFormat = writer.getFormat();
	} else if (options.outputFormat == VTF::FORMAT_DEFAULT) {
		options.outputFormat = writer.getDefaultFormat();
	}
	if (options.createMips) {
		writer.setMipCount(ImageDimensions::getRecommendedMipCountForDims(options.outputFormat, writer.getWidth(), writer.getHeight()));
		writer.computeMips();
	}
	writer.setFormat(options.outputFormat);
	writer.setCompressionLevel(options.compressionLevel);
	writer.setCompressionMethod(options.compressionMethod);
}

void VTF::create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, CreationOptions options) {
	VTF writer;
	writer.setVersion(options.majorVersion, options.minorVersion);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	writer.setImage(imageData, format, width, height, options.filter);
	createInternal(writer, options);
	writer.bake(vtfPath);
}

void VTF::create(ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, CreationOptions options) {
	std::vector<std::byte> imageData;
	imageData.resize(width * height * (ImageFormatDetails::bpp(format) / 8));
	create(imageData, format, width, height, vtfPath, options);
}

VTF VTF::create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, CreationOptions options) {
	VTF writer;
	writer.setVersion(options.majorVersion, options.minorVersion);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	writer.setImage(imageData, format, width, height, options.filter);
	createInternal(writer, options);
	return writer;
}

VTF VTF::create(ImageFormat format, uint16_t width, uint16_t height, CreationOptions options) {
	std::vector<std::byte> imageData;
	imageData.resize(width * height * (ImageFormatDetails::bpp(format) / 8));
	return create(imageData, format, width, height, options);
}

void VTF::create(const std::string& imagePath, const std::string& vtfPath, CreationOptions options) {
	VTF writer;
	writer.setVersion(options.majorVersion, options.minorVersion);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	writer.setImage(imagePath, options.filter);
	createInternal(writer, options);
	writer.bake(vtfPath);
}

VTF VTF::create(const std::string& imagePath, CreationOptions options) {
	VTF writer;
	writer.setVersion(options.majorVersion, options.minorVersion);
	writer.addFlags(options.flags);
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	writer.setImage(imagePath, options.filter);
	createInternal(writer, options);
	return writer;
}

uint32_t VTF::getMajorVersion() const {
	return this->majorVersion;
}

uint32_t VTF::getMinorVersion() const {
	return this->minorVersion;
}

void VTF::setVersion(uint32_t newMajorVersion, uint32_t newMinorVersion) {
	this->setMajorVersion(newMajorVersion);
	this->setMinorVersion(newMinorVersion);
}

void VTF::setMajorVersion(uint32_t newMajorVersion) {
	this->majorVersion = newMajorVersion;
}

void VTF::setMinorVersion(uint32_t newMinorVersion) {
	if (this->hasImageData()) {
		auto faceCount = this->getFaceCount();
		if (faceCount == 7 && (newMinorVersion < 1 || newMinorVersion > 4)) {
			faceCount = 6;
		}
		this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, faceCount, this->sliceCount);
	}
	this->minorVersion = newMinorVersion;
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
		if (const auto recommendedCount = ImageDimensions::getRecommendedMipCountForDims(this->format, newWidth, newHeight); newMipCount > recommendedCount) {
			newMipCount = recommendedCount;
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

VTF::Flags VTF::getFlags() const {
	return this->flags;
}

void VTF::setFlags(Flags flags_) {
	this->flags = static_cast<Flags>((this->flags & FLAG_MASK_GENERATED) | (flags_ & ~FLAG_MASK_GENERATED));
}

void VTF::addFlags(Flags flags_) {
	this->flags |= static_cast<Flags>(flags_ & ~FLAG_MASK_GENERATED);
}

void VTF::removeFlags(Flags flags_) {
	this->flags &= static_cast<Flags>(~flags_ | FLAG_MASK_GENERATED);
}

ImageFormat VTF::getFormat() const {
	return this->format;
}

void VTF::setFormat(ImageFormat newFormat, ImageConversion::ResizeFilter filter) {
	if (newFormat == VTF::FORMAT_UNCHANGED || newFormat == this->format) {
		return;
	}
	if (newFormat == VTF::FORMAT_DEFAULT) {
		newFormat = this->getDefaultFormat();
	}
	if (!this->hasImageData()) {
		this->format = newFormat;
		return;
	}
	auto newMipCount = this->mipCount;
	if (const auto recommendedCount = ImageDimensions::getRecommendedMipCountForDims(newFormat, this->width, this->height); newMipCount > recommendedCount) {
		newMipCount = recommendedCount;
	}
	if (ImageFormatDetails::compressed(newFormat)) {
		this->regenerateImageData(newFormat, this->width + math::paddingForAlignment(4, this->width), this->height + math::paddingForAlignment(4, this->height), newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount, filter);
	} else {
		this->regenerateImageData(newFormat, this->width, this->height, newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount, filter);
	}
}

uint8_t VTF::getMipCount() const {
	return this->mipCount;
}

bool VTF::setMipCount(uint8_t newMipCount) {
	if (!this->hasImageData()) {
		return false;
	}
	if (const auto recommended = ImageDimensions::getRecommendedMipCountForDims(this->format, this->width, this->height); newMipCount > recommended) {
		newMipCount = recommended;
		if (newMipCount == 1) {
			return false;
		}
	}
	if (newMipCount > 1) {
		this->flags &= ~(FLAG_NO_MIP | FLAG_NO_LOD);
	} else {
		this->flags |= FLAG_NO_MIP | FLAG_NO_LOD;
	}
	this->regenerateImageData(this->format, this->width, this->height, newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount);
	return true;
}

bool VTF::setRecommendedMipCount() {
	return this->setMipCount(ImageDimensions::getRecommendedMipCountForDims(this->format, this->width, this->height));
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
	futures.reserve((this->mipCount - 1) * this->frameCount * faceCount * this->sliceCount);
#endif
	for (int j = 0; j < this->frameCount; j++) {
		for (int k = 0; k < faceCount; k++) {
			for (int l = 0; l < this->sliceCount; l++) {
#ifdef SOURCEPP_BUILD_WITH_THREADS
				futures.push_back(std::async(std::launch::async, [this, filter, outputDataPtr, faceCount, j, k, l] {
#endif
					for (int i = 1; i < this->mipCount; i++) {
						auto mip = ImageConversion::resizeImageData(this->getImageDataRaw(i - 1, j, k, l), this->format, ImageDimensions::getMipDim(i - 1, this->width), ImageDimensions::getMipDim(i, this->width), ImageDimensions::getMipDim(i - 1, this->height), ImageDimensions::getMipDim(i, this->height), this->imageDataIsSRGB(), filter);
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
	const auto expectedLength = ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, 6, this->width, this->height, this->sliceCount);
	if (this->majorVersion == 7 && this->minorVersion >= 1 && this->minorVersion <= 4 && expectedLength < image->data.size()) {
		return 7;
	}
	if (expectedLength == image->data.size()) {
		return 6;
	}
	return 1;
}

bool VTF::setFaceCount(bool isCubemap, bool hasSphereMap) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, isCubemap ? ((this->minorVersion >= 1 && this->minorVersion <= 4 && hasSphereMap) ? 7 : 6) : 1, this->sliceCount);
	return true;
}

/*
bool VTF::computeSphereMap() {
	if (this->getFaceCount() < 7) {
		return false;
	}
	// compute spheremap here
	return true;
}
*/

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

bool VTF::setFrameFaceAndSliceCount(uint16_t newFrameCount, bool isCubemap, bool hasSphereMap, uint16_t newSliceCount) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, newFrameCount, isCubemap ? ((this->minorVersion >= 1 && this->minorVersion <= 4 && hasSphereMap) ? 7 : 6) : 1, newSliceCount);
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
		return out / (rgba8888Data.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
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
	for (const auto& [type, flags, data] : this->resources) {
		resourceData[type] = {std::vector<std::byte>{data.begin(), data.end()}, 0};
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

	for (auto& [type, flags, data] : this->resources) {
		if (resourceData.contains(type)) {
			const auto& [specificResourceData, offset] = resourceData[type];
			data = {this->data.data() + offset, specificResourceData.size()};
		}
	}
}

void VTF::removeResourceInternal(Resource::Type type) {
	std::erase_if(this->resources, [type](const Resource& resource) { return resource.type == type; });
}

void VTF::regenerateImageData(ImageFormat newFormat, uint16_t newWidth, uint16_t newHeight, uint8_t newMipCount, uint16_t newFrameCount, uint8_t newFaceCount, uint16_t newSliceCount, ImageConversion::ResizeFilter filter) {
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

	std::vector<std::byte> newImageData;
	if (const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA); imageResource && this->hasImageData()) {
		if (this->format != newFormat && this->width == newWidth && this->height == newHeight && this->mipCount == newMipCount && this->frameCount == newFrameCount && faceCount == newFaceCount && this->sliceCount == newSliceCount) {
			newImageData = ImageConversion::convertSeveralImageDataToFormat(imageResource->data, this->format, newFormat, this->mipCount, this->frameCount, faceCount, this->width, this->height, this->sliceCount);
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
									image = ImageConversion::resizeImageData(image, this->format, ImageDimensions::getMipDim(i, this->width), ImageDimensions::getMipDim(i, newWidth), ImageDimensions::getMipDim(i, this->height), ImageDimensions::getMipDim(i, newHeight), this->imageDataIsSRGB(), filter);
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
				newImageData = ImageConversion::convertSeveralImageDataToFormat(newImageData, this->format, newFormat, newMipCount, newFrameCount, newFaceCount, newWidth, newHeight, newSliceCount);
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

void VTF::setParticleSheetResource(std::span<const std::byte> value) {
	this->setResourceInternal(Resource::TYPE_PARTICLE_SHEET_DATA, value);
}

void VTF::removeParticleSheetResource() {
	this->removeResourceInternal(Resource::TYPE_PARTICLE_SHEET_DATA);
}

void VTF::setCRCResource(uint32_t value) {
	std::vector<std::byte> crcData;
	BufferStream writer{crcData};

	writer.write<uint32_t>(value);
	crcData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_CRC, crcData);
}

void VTF::removeCRCResource() {
	this->removeResourceInternal(Resource::TYPE_CRC);
}

void VTF::setLODResource(uint8_t u, uint8_t v) {
	std::vector<std::byte> lodData;
	BufferStream writer{lodData};

	writer
		.write<uint8_t>(u)
		.write<uint8_t>(v)
		.write<uint16_t>(0);
	lodData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_LOD_CONTROL_INFO, lodData);
}

void VTF::removeLODResource() {
	this->removeResourceInternal(Resource::TYPE_LOD_CONTROL_INFO);
}

void VTF::setExtendedFlagsResource(uint32_t value) {
	std::vector<std::byte> extendedFlagsData;
	BufferStream writer{extendedFlagsData};

	writer.write<uint32_t>(value);
	extendedFlagsData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_EXTENDED_FLAGS, extendedFlagsData);
}

void VTF::removeExtendedFlagsResource() {
	this->removeResourceInternal(Resource::TYPE_EXTENDED_FLAGS);
}

void VTF::setKeyValuesDataResource(const std::string& value) {
	std::vector<std::byte> keyValuesData;
	BufferStream writer{keyValuesData};

	writer.write<uint32_t>(value.size());
	writer.write(value, false);
	keyValuesData.resize(writer.size());

	this->setResourceInternal(Resource::TYPE_KEYVALUES_DATA, keyValuesData);
}

void VTF::removeKeyValuesDataResource() {
	this->removeResourceInternal(Resource::TYPE_KEYVALUES_DATA);
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
	this->compressionMethod = newCompressionMethod;
}

bool VTF::hasImageData() const {
	return this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0;
}

bool VTF::imageDataIsSRGB() const {
	return !ImageFormatDetails::large(this->format) && (this->flags & FLAG_SRGB);
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

bool VTF::setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
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
		if (face > 6 || (face == 6 && (this->minorVersion < 1 || this->minorVersion > 4))) {
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
			image = ImageConversion::resizeImageData(image, format_, width_, newWidth, height_, newHeight, this->imageDataIsSRGB(), filter);
		}
		if (format_ != this->format) {
			image = ImageConversion::convertImageDataToFormat(image, format_, this->format, newWidth, newHeight);
		}
		std::memcpy(imageResource->data.data() + offset, image.data(), image.size());
	}
	return true;
}

bool VTF::setImage(const std::string& imagePath, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	ImageFormat inputFormat;
	int inputWidth, inputHeight, inputFrameCount;
	auto imageData_ = ImageConversion::convertFileToImageData(fs::readFileBuffer(imagePath), inputFormat, inputWidth, inputHeight, inputFrameCount);

	// Unable to decode file
	if (imageData_.empty() || inputFormat == ImageFormat::EMPTY || !inputWidth || !inputHeight || !inputFrameCount) {
		return false;
	}

	// One frame (normal)
	if (inputFrameCount == 1) {
		return this->setImage(imageData_, inputFormat, inputWidth, inputHeight, filter, mip, frame, face, slice);
	}

	// Multiple frames (GIF)
	bool allSuccess = true;
	const auto frameSize = ImageFormatDetails::getDataLength(inputFormat, inputWidth, inputHeight);
	for (int currentFrame = 0; currentFrame < inputFrameCount; currentFrame++) {
		if (!this->setImage({imageData_.data() + currentFrame * frameSize, imageData_.data() + currentFrame * frameSize + frameSize}, inputFormat, inputWidth, inputHeight, filter, mip, frame + currentFrame, face, slice)) {
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

void VTF::setThumbnail(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_) {
	if (format_ != this->thumbnailFormat) {
		this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, ImageConversion::convertImageDataToFormat(imageData_, format_, this->thumbnailFormat, width_, height_));
	} else {
		this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, imageData_);
	}
	this->thumbnailWidth = width_;
	this->thumbnailHeight = height_;
}

void VTF::computeThumbnail(ImageConversion::ResizeFilter filter) {
	if (!this->hasImageData()) {
		return;
	}
	this->thumbnailFormat = ImageFormat::DXT1;
	this->thumbnailWidth = 16;
	this->thumbnailHeight = 16;
	this->setResourceInternal(Resource::TYPE_THUMBNAIL_DATA, ImageConversion::convertImageDataToFormat(ImageConversion::resizeImageData(this->getImageDataRaw(), this->format, this->width, this->thumbnailWidth, this->height, this->thumbnailHeight, this->flags & FLAG_SRGB, filter), this->format, this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight));
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

std::vector<std::byte> VTF::bake() const {
	std::vector<std::byte> out;
	BufferStream writer{out};

	writer << VTF_SIGNATURE << this->majorVersion << this->minorVersion;
	const auto headerLengthPos = writer.tell();
	writer.write<uint32_t>(0);

	writer
		.write(this->width)
		.write(this->height)
		.write(this->flags)
		.write(this->frameCount)
		.write(this->startFrame)
		.write<uint32_t>(0) // padding
		.write(this->reflectivity)
		.write<uint32_t>(0) // padding
		.write(this->bumpMapScale)
		.write(this->format)
		.write(this->mipCount)
		.write(ImageFormat::DXT1)
		.write(this->thumbnailWidth)
		.write(this->thumbnailHeight);

	if (this->minorVersion >= 2) {
		writer << this->sliceCount;
	}

	if (this->minorVersion < 3) {
		const auto headerAlignment = math::paddingForAlignment(16, writer.tell());
		for (uint16_t i = 0; i < headerAlignment; i++) {
			writer.write<std::byte>({});
		}
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
			hasAuxCompression = this->minorVersion >= 6 && this->compressionLevel != 0;
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

		writer
			.write<uint8_t>(0) // padding
			.write<uint8_t>(0) // padding
			.write<uint8_t>(0) // padding
			.write<uint32_t>(this->getResources().size() + hasAuxCompression)
			.write<uint64_t>(0); // padding

		const auto resourceStart = writer.tell();
		const auto headerSize = resourceStart + ((this->getResources().size() + hasAuxCompression) * sizeof(uint64_t));
		writer.seek_u(headerLengthPos).write<uint32_t>(headerSize).seek_u(resourceStart);
		while (writer.tell() < headerSize) {
			writer.write<uint64_t>(0);
		}
		writer.seek_u(resourceStart);

		static constexpr auto writeNonLocalResource = [](BufferStream& writer_, Resource::Type type, std::span<const std::byte> data) {
			writer_.write<uint32_t>(type);
			const auto resourceOffsetPos = writer_.tell();
			writer_.seek(0, BufferStream::SEEKDIR_END);
			const auto resourceOffsetValue = writer_.tell();
			writer_.write(data);
			writer_.seek_u(resourceOffsetPos).write<uint32_t>(resourceOffsetValue);
		};
		for (const auto resourceType : Resource::getOrder()) {
			if (hasAuxCompression && resourceType == Resource::TYPE_AUX_COMPRESSION) {
				writeNonLocalResource(writer, resourceType, auxCompressionResourceData);
			} else if (hasAuxCompression && resourceType == Resource::TYPE_IMAGE_DATA) {
				writeNonLocalResource(writer, resourceType, compressedImageResourceData);
			} else if (const auto* resource = this->getResource(resourceType)) {
				if ((resource->flags & Resource::FLAG_LOCAL_DATA) && resource->data.size() == sizeof(uint32_t)) {
					writer.write<uint32_t>((Resource::FLAG_LOCAL_DATA << 24) | resource->type);
					writer.write(resource->data);
				} else {
					writeNonLocalResource(writer, resource->type, resource->data);
				}
			}
		}
	}

	out.resize(writer.size());
	return out;
}

bool VTF::bake(const std::string& vtfPath) const {
	return fs::writeFileBuffer(vtfPath, this->bake());
}
