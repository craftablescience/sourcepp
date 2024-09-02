#include <vtfpp/vtfpp.h>

#include <cstring>
#include <memory>

#include <BufferStream.h>
#include <miniz.h>
#include <stb_image.h>

#include <vtfpp/ImageConversion.h>

using namespace sourcepp;
using namespace vtfpp;

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
			return std::span<uint32_t>{reinterpret_cast<uint32_t*>(this->data.data()), this->data.size() / 4};
		default:
			break;
	}
	return {};
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

	auto headerLength = stream.read<uint32_t>();

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
		// todo: when loading a compressed vtf, decompress it so the raw getter works

		stream.skip(3);
		auto resourceCount = stream.read<uint32_t>();
		stream.skip(8);

		if (resourceCount > VTF_MAX_RESOURCES) {
			resourceCount = VTF_MAX_RESOURCES;
		}
		this->resources.reserve(resourceCount);

		Resource* lastResource = nullptr;
		for (int i = 0; i < resourceCount; i++) {
			auto& resource = this->resources.emplace_back();

			auto typeAndFlags = stream.read<uint32_t>();
			resource.type = static_cast<Resource::Type>(typeAndFlags & 0xffffff); // last 3 bytes
			resource.flags = static_cast<Resource::Flags>(typeAndFlags >> 24); // first byte
			resource.data = stream.read_span<std::byte>(4);

			if (!(resource.flags & Resource::FLAG_LOCAL_DATA)) {
				if (lastResource) {
					auto lastOffset = *reinterpret_cast<uint32_t*>(lastResource->data.data());
					auto currentOffset = *reinterpret_cast<uint32_t*>(resource.data.data());

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

		if (auto auxResource = this->getResource(Resource::TYPE_AUX_COMPRESSION)) {
			this->hasAuxCompression = auxResource->getDataAsAuxCompressionLevel() != 0;
		}

		this->opened = stream.tell() == headerLength;
	} else {
		stream.skip(math::getPaddingForAlignment(16, stream.tell()));
		this->opened = stream.tell() == headerLength;

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
				.data = stream.read_span<std::byte>(ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount)),
			});
		}
	}
}

VTF::VTF(std::span<const std::byte> vtfData, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{vtfData.begin(), vtfData.end()}, parseHeaderOnly) {}

VTF::VTF(const std::string& vtfPath, bool parseHeaderOnly)
		: VTF(fs::readFileBuffer(vtfPath), parseHeaderOnly) {}

VTF::operator bool() const {
	return this->opened;
}

std::span<const std::byte> VTF::getData() const {
	return this->data;
}

uint32_t VTF::getMajorVersion() const {
	return this->majorVersion;
}

uint32_t VTF::getMinorVersion() const {
	return this->minorVersion;
}

uint16_t VTF::getWidth(uint8_t mip) const {
	return mip > 0 ? ImageDimensions::getMipDim(mip, this->width) : this->width;
}

uint16_t VTF::getHeight(uint8_t mip) const {
	return mip > 0 ? ImageDimensions::getMipDim(mip, this->height) : this->height;
}

VTF::Flags VTF::getFlags() const {
	return this->flags;
}

ImageFormat VTF::getFormat() const {
	return this->format;
}

uint8_t VTF::getMipCount() const {
	return this->mipCount;
}

uint16_t VTF::getFrameCount() const {
	return this->frameCount;
}

uint8_t VTF::getFaceCount() const {
	return getFaceCountFor(this->majorVersion, this->minorVersion, this->flags, this->startFrame);
}

uint8_t VTF::getFaceCountFor(uint32_t majorVersion, uint32_t minorVersion, Flags flags, uint16_t startFrame) {
	return (flags & VTF::FLAG_ENVMAP) ? ((majorVersion <= 7 && minorVersion < 5 && startFrame == 0xffff) ? 7 : 6) : 1;
}

uint16_t VTF::getSliceCount() const {
	return this->sliceCount;
}

uint16_t VTF::getStartFrame() const {
	return this->startFrame;
}

math::Vec3f VTF::getReflectivity() const {
	return this->reflectivity;
}

float VTF::getBumpMapScale() const {
	return this->bumpMapScale;
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

bool VTF::isCompressed() const {
	return this->hasAuxCompression;
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

bool VTF::hasImageData() const {
	return this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0;
}

std::span<const std::byte> VTF::getImageDataRaw(uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	auto imageResource = this->getResource(Resource::TYPE_IMAGE_DATA);
	if (!imageResource) {
		return {};
	}

	uint32_t offset, length;
	if (this->hasAuxCompression) {
		auto* auxResource = this->getResource(Resource::TYPE_AUX_COMPRESSION);
		if (!auxResource) {
			return {};
		}

		// Keep in mind that the slice parameter gets ignored when returning raw compressed data - the slices are compressed together
		offset = 0;
		for (uint8_t i = this->mipCount - 1; i >= 0; i--) {
			for (uint16_t j = 0; j < this->frameCount; j++) {
				for (uint8_t k = 0; k < this->getFaceCount(); k++) {
					length = auxResource->getDataAsAuxCompressionLength(i, this->mipCount, j, this->frameCount, k, this->getFaceCount());
					if (i == mip && j == frame && k == face) {
						return imageResource->data.subspan(offset, length);
					} else {
						offset += length;
					}
				}
			}
		}
	} else if (ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, frame, this->frameCount, face, this->getFaceCount(), this->width, this->height, slice, this->sliceCount)) {
		return imageResource->data.subspan(offset, length);
	}
	return {};
}

std::vector<std::byte> VTF::getImageDataAs(ImageFormat newFormat, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	auto rawImageData = this->getImageDataRaw(mip, frame, face, slice);
	if (rawImageData.empty()) {
		return {};
	}

	if (!this->hasAuxCompression) {
		return ImageConversion::convertImageDataToFormat(rawImageData, this->format, newFormat, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
	}

	auto* auxResource = this->getResource(Resource::TYPE_AUX_COMPRESSION);
	if (!auxResource) {
		return {};
	}

	// This section can be changed to limit realloc's but I don't know how and I don't care
	mz_ulong decompressedImageDataSize = rawImageData.size();
	std::vector<std::byte> decompressedImageData;
	int mzStatus;
	do {
		decompressedImageDataSize *= 2;
		decompressedImageData.resize(decompressedImageDataSize);
		mzStatus = mz_uncompress(reinterpret_cast<unsigned char*>(decompressedImageData.data()), &decompressedImageDataSize, reinterpret_cast<const unsigned char*>(rawImageData.data()), rawImageData.size());
	} while (mzStatus == MZ_BUF_ERROR);

	if (mzStatus != MZ_OK) {
		return {};
	}
	decompressedImageData.resize(decompressedImageDataSize);

	// 3D compressed images will have their slices aggregated together, so we need to do some trickery to pull a specific one out
	if (this->sliceCount == 1) {
		return ImageConversion::convertImageDataToFormat(decompressedImageData, this->format, newFormat, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
	}
	const auto sliceSize = ImageFormatDetails::getDataLength(this->format, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height), 1);
	return ImageConversion::convertImageDataToFormat({decompressedImageData.begin() + (slice * sliceSize), sliceSize}, this->format, newFormat, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
}

std::vector<std::byte> VTF::getImageDataAsRGBA8888(uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	return this->getImageDataAs(ImageFormat::RGBA8888, mip, frame, face, slice);
}

std::vector<std::byte> VTF::convertAndSaveImageDataToFile(uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) const {
	std::span<const std::byte> rawImageData;
	std::vector<std::byte> decompressedImageData;
	if (this->hasAuxCompression) {
		decompressedImageData = this->getImageDataAs(this->format, mip, frame, face, slice);
		rawImageData = {decompressedImageData.begin(), decompressedImageData.end()};
	} else {
		rawImageData = this->getImageDataRaw(mip, frame, face, slice);
	}

	if (rawImageData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFile(rawImageData, this->format, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
}

bool VTF::hasThumbnailData() const {
	return this->thumbnailFormat != ImageFormat::EMPTY && this->thumbnailWidth > 0 && this->thumbnailHeight > 0;
}

std::span<const std::byte> VTF::getThumbnailDataRaw() const {
	if (auto thumbnailResource = this->getResource(Resource::TYPE_THUMBNAIL_DATA)) {
		return thumbnailResource->data;
	}
	return {};
}

std::vector<std::byte> VTF::getThumbnailDataAs(ImageFormat newFormat) const {
	auto rawThumbnailData = this->getThumbnailDataRaw();
	if (rawThumbnailData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFormat(rawThumbnailData, this->thumbnailFormat, newFormat, this->thumbnailWidth, this->thumbnailHeight);
}

std::vector<std::byte> VTF::getThumbnailDataAsRGBA8888() const {
	return this->getThumbnailDataAs(ImageFormat::RGBA8888);
}

std::vector<std::byte> VTF::convertAndSaveThumbnailDataToFile() const {
	auto rawThumbnailData = this->getThumbnailDataRaw();
	if (rawThumbnailData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFile(rawThumbnailData, this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight);
}

VTFWriter::VTFWriter()
		: VTF() {
	this->opened = true;

	this->majorVersion = 7;
	this->minorVersion = 4;

	this->format = ImageFormat::EMPTY;
	this->thumbnailFormat = ImageFormat::EMPTY;
}

VTFWriter::VTFWriter(std::vector<std::byte>&& vtfData)
		: VTF(std::move(vtfData)) {
	// This is kind of evil, but it's fine :3
	if (const auto* resource = this->getResource(Resource::TYPE_THUMBNAIL_DATA); resource && this->thumbnailFormat != ImageFormat::EMPTY && this->thumbnailWidth > 0 && this->thumbnailHeight > 0) {
		this->thumbnailData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_IMAGE_DATA); resource && this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0) {
		this->imageData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_PARTICLE_SHEET_DATA)) {
		this->particleSheetData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_CRC)) {
		this->crcData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_LOD_CONTROL_INFO)) {
		this->lodData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_EXTENDED_FLAGS)) {
		this->extendedFlagsData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_KEYVALUES_DATA)) {
		this->keyValuesData = {resource->data.begin(), resource->data.end()};
	}
	if (const auto* resource = this->getResource(Resource::TYPE_AUX_COMPRESSION)) {
		this->compressionLevel = static_cast<int8_t>(resource->getDataAsAuxCompressionLevel());
	}
}

VTFWriter::VTFWriter(const std::string& vtfPath)
		: VTFWriter(fs::readFileBuffer(vtfPath)) {}

void VTFWriter::create(std::vector<std::byte>&& imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, uint32_t majorVersion, uint32_t minorVersion, ImageConversion::ResizeMethod widthResizeMethod, ImageConversion::ResizeMethod heightResizeMethod, ImageConversion::ResizeFilter filter, bool createMips, bool createThumbnail, Flags flags, float bumpMapScale) {
	VTFWriter writer;
	writer.setVersion(majorVersion, minorVersion);
	writer.setImageResizeMethods(widthResizeMethod, heightResizeMethod);
	writer.setImage(imageData, format, width, height, filter);
	if (createMips) {
		writer.computeMips();
	}
	if (createThumbnail) {
		writer.computeThumbnail();
	}
	writer.addFlags(flags);
	writer.setBumpMapScale(bumpMapScale);
	writer.bake(vtfPath);
}

VTF VTFWriter::create(std::vector<std::byte>&& imageData, ImageFormat format, uint16_t width, uint16_t height, uint32_t majorVersion, uint32_t minorVersion, ImageConversion::ResizeMethod widthResizeMethod, ImageConversion::ResizeMethod heightResizeMethod, ImageConversion::ResizeFilter filter, bool createMips, bool createThumbnail, Flags flags, float bumpMapScale) {
	VTFWriter writer;
	writer.setVersion(majorVersion, minorVersion);
	writer.setImageResizeMethods(widthResizeMethod, heightResizeMethod);
	writer.setImage(imageData, format, width, height, filter);
	if (createMips) {
		writer.computeMips();
	}
	if (createThumbnail) {
		writer.computeThumbnail();
	}
	writer.addFlags(flags);
	writer.setBumpMapScale(bumpMapScale);
	return VTF{writer.bake()};
}

void VTFWriter::create(const std::string& imagePath, const std::string& vtfPath, uint32_t majorVersion, uint32_t minorVersion, ImageConversion::ResizeMethod widthResizeMethod, ImageConversion::ResizeMethod heightResizeMethod, ImageConversion::ResizeFilter filter, bool createMips, bool createThumbnail, Flags flags, float bumpMapScale) {
	VTFWriter writer;
	writer.setVersion(majorVersion, minorVersion);
	writer.setImageResizeMethods(widthResizeMethod, heightResizeMethod);
	writer.setImage(imagePath, filter);
	if (createMips) {
		writer.computeMips();
	}
	if (createThumbnail) {
		writer.computeThumbnail();
	}
	writer.addFlags(flags);
	writer.setBumpMapScale(bumpMapScale);
	writer.bake(vtfPath);
}

VTF VTFWriter::create(const std::string& imagePath, uint32_t majorVersion, uint32_t minorVersion, ImageConversion::ResizeMethod widthResizeMethod, ImageConversion::ResizeMethod heightResizeMethod, ImageConversion::ResizeFilter filter, bool createMips, bool createThumbnail, Flags flags, float bumpMapScale) {
	VTFWriter writer;
	writer.setVersion(majorVersion, minorVersion);
	writer.setImageResizeMethods(widthResizeMethod, heightResizeMethod);
	writer.setImage(imagePath, filter);
	if (createMips) {
		writer.computeMips();
	}
	if (createThumbnail) {
		writer.computeThumbnail();
	}
	writer.addFlags(flags);
	writer.setBumpMapScale(bumpMapScale);
	return VTF{writer.bake()};
}

void VTFWriter::setVersion(uint32_t newMajorVersion, uint32_t newMinorVersion) {
	this->setMajorVersion(newMajorVersion);
	this->setMinorVersion(newMinorVersion);
}

void VTFWriter::setMajorVersion(uint32_t newMajorVersion) {
	this->majorVersion = newMajorVersion;
}

void VTFWriter::setMinorVersion(uint32_t newMinorVersion) {
	if (this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0) {
		this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, getFaceCountFor(this->majorVersion, newMinorVersion, this->flags, this->startFrame), this->sliceCount);
	}
	this->minorVersion = newMinorVersion;
}

ImageConversion::ResizeMethod VTFWriter::getImageWidthResizeMethod() const {
	return this->imageWidthResizeMethod;
}

ImageConversion::ResizeMethod VTFWriter::getImageHeightResizeMethod() const {
	return this->imageHeightResizeMethod;
}

void VTFWriter::setImageResizeMethods(ImageConversion::ResizeMethod imageWidthResizeMethod_, ImageConversion::ResizeMethod imageHeightResizeMethod_) {
	this->imageWidthResizeMethod = imageWidthResizeMethod_;
	this->imageHeightResizeMethod = imageHeightResizeMethod_;
}

void VTFWriter::setSize(uint16_t newWidth, ImageConversion::ResizeMethod widthResize, uint16_t newHeight, ImageConversion::ResizeMethod heightResize, ImageConversion::ResizeFilter filter) {
	if (newWidth == 0 || newHeight == 0) {
		this->imageData.clear();
		this->format = ImageFormat::EMPTY;
		this->width = 0;
		this->height = 0;
		return;
	}

	ImageConversion::setResizedDims(newWidth, widthResize, newHeight, heightResize);
	if (this->hasImageData()) {
		this->regenerateImageData(this->format, newWidth, newHeight, this->mipCount, this->frameCount, this->getFaceCount(), this->sliceCount, filter);
	} else {
		this->format = ImageFormat::RGBA8888;
		this->width = newWidth;
		this->height = newHeight;
		this->imageData.resize(ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount));
	}
}

void VTFWriter::setFlags(Flags flags_) {
	this->flags = static_cast<Flags>((this->flags & FLAG_MASK_GENERATED) | (flags_ & ~FLAG_MASK_GENERATED));
}

void VTFWriter::addFlags(Flags flags_) {
	this->flags |= static_cast<Flags>(flags_ & ~FLAG_MASK_GENERATED);
}

void VTFWriter::removeFlags(Flags flags_) {
	this->flags &= static_cast<Flags>(~flags_ | FLAG_MASK_GENERATED);
}

void VTFWriter::setFormat(ImageFormat newFormat) {
	if (!this->hasImageData()) {
		this->format = newFormat;
		return;
	}
	this->regenerateImageData(newFormat, this->width, this->height, this->mipCount, this->frameCount, this->getFaceCount(), this->sliceCount);
}

bool VTFWriter::setMipCount(uint8_t newMipCount) {
	if (!this->hasImageData()) {
		return false;
	}
	if (newMipCount > ImageDimensions::getRecommendedMipCountForDims(this->width, this->height)) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, newMipCount, this->frameCount, this->getFaceCount(), this->sliceCount);
	return true;
}

bool VTFWriter::setRecommendedMipCount() {
	return this->setMipCount(ImageDimensions::getRecommendedMipCountForDims(this->width, this->height));
}

void VTFWriter::computeMips(ImageConversion::ResizeFilter filter) {
	if (this->mipCount == 1) {
		this->setRecommendedMipCount();
	}
	// todo(vtfpp): compute mips
}

bool VTFWriter::setFrameCount(uint16_t newFrameCount) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, newFrameCount, this->getFaceCount(), this->sliceCount);
	return true;
}

bool VTFWriter::setFaceCount(bool hasMultipleFaces) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, getFaceCountFor(this->majorVersion, this->minorVersion, hasMultipleFaces ? this->flags | FLAG_ENVMAP : this->flags & ~FLAG_ENVMAP, this->startFrame), this->sliceCount);
	return true;
}

bool VTFWriter::setSliceCount(uint16_t newSliceCount) {
	if (!this->hasImageData()) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, this->mipCount, this->frameCount, this->getFaceCount(), newSliceCount);
	return true;
}

bool VTFWriter::setStartFrame(uint16_t newStartFrame) {
	if (this->majorVersion <= 7 && this->minorVersion < 5 && (this->flags & FLAG_ENVMAP) && ((this->startFrame == 0xffff && newStartFrame != 0xffff) || (this->startFrame != 0xffff && newStartFrame == 0xffff))) {
		this->startFrame = newStartFrame;
		return this->setFaceCount(true);
	}
	this->startFrame = newStartFrame;
	return true;
}

void VTFWriter::setReflectivity(sourcepp::math::Vec3f newReflectivity) {
	this->reflectivity = newReflectivity;
}

void VTFWriter::computeReflectivity() {
	// https://stackoverflow.com/a/56678483
	// todo(vtfpp): compute reflectivity (luminance)
}

void VTFWriter::setBumpMapScale(float newBumpMapScale) {
	this->bumpMapScale = newBumpMapScale;
}

void VTFWriter::computeThumbnail(ImageConversion::ResizeFilter filter) {
	this->thumbnailFormat = ImageFormat::DXT1;
	this->thumbnailWidth = 16;
	this->thumbnailHeight = 16;
	this->thumbnailData = ImageConversion::convertImageDataToFormat(ImageConversion::resizeImageData(this->getImageDataRaw(), this->format, this->width, this->thumbnailWidth, this->height, this->thumbnailHeight, this->flags & FLAG_SRGB, filter), this->format, this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight);

	bool foundResource = false;
	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_THUMBNAIL_DATA) {
			resource.data = this->thumbnailData;
			foundResource = true;
			break;
		}
	}
	if (!foundResource) {
		this->resources.push_back({
			.type = Resource::TYPE_THUMBNAIL_DATA,
			.flags = Resource::FLAG_NONE,
			.data = this->thumbnailData,
		});
	}
}

bool VTFWriter::computeSphereMap() {
	if (this->getFaceCount() != 7) {
		return false;
	}
	// todo(vtfpp): compute sphere map
	return false;
}

void VTFWriter::setParticleSheetResource(std::vector<std::byte>&& value) {
	this->particleSheetData = value;

	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_PARTICLE_SHEET_DATA) {
			resource.data = this->particleSheetData;
			return;
		}
	}
	this->resources.push_back({
		.type = Resource::TYPE_PARTICLE_SHEET_DATA,
		.flags = Resource::FLAG_NONE,
		.data = this->particleSheetData,
	});
}

void VTFWriter::setCRCResource(uint32_t value) {
	this->crcData.clear();
	BufferStream writer{this->crcData};

	writer.write<uint32_t>(value);
	this->crcData.resize(writer.size());

	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_CRC) {
			resource.data = this->crcData;
			return;
		}
	}
	this->resources.push_back({
		.type = Resource::TYPE_CRC,
		.flags = Resource::FLAG_LOCAL_DATA,
		.data = this->crcData,
	});
}

bool VTFWriter::getComputeCRCResourceForBake() const {
	return this->computeCRCOnBake;
}

void VTFWriter::setComputeCRCResourceForBake(bool computeCRCResourceOnBake) {
	this->computeCRCOnBake = computeCRCResourceOnBake;
}

void VTFWriter::setLODResource(uint8_t u, uint8_t v) {
	this->lodData.clear();
	BufferStream writer{this->lodData};

	writer
		.write<uint8_t>(u)
		.write<uint8_t>(v)
		.write<uint16_t>(0);
	this->lodData.resize(writer.size());

	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_LOD_CONTROL_INFO) {
			resource.data = this->lodData;
			return;
		}
	}
	this->resources.push_back({
		.type = Resource::TYPE_LOD_CONTROL_INFO,
		.flags = Resource::FLAG_LOCAL_DATA,
		.data = this->lodData,
	});
}

void VTFWriter::setExtendedFlagsResource(uint32_t value) {
	this->extendedFlagsData.clear();
	BufferStream writer{this->extendedFlagsData};

	writer.write<uint32_t>(value);
	this->extendedFlagsData.resize(writer.size());

	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_EXTENDED_FLAGS) {
			resource.data = this->extendedFlagsData;
			return;
		}
	}
	this->resources.push_back({
		.type = Resource::TYPE_EXTENDED_FLAGS,
		.flags = Resource::FLAG_LOCAL_DATA,
		.data = this->extendedFlagsData,
	});
}

void VTFWriter::setKeyValuesData(const std::string& value) {
	this->keyValuesData.clear();
	BufferStream writer{this->keyValuesData};

	writer.write<uint32_t>(value.size());
	writer.write(value, false);
	this->keyValuesData.resize(writer.size());

	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_KEYVALUES_DATA) {
			resource.data = this->keyValuesData;
			return;
		}
	}
	this->resources.push_back({
		.type = Resource::TYPE_KEYVALUES_DATA,
		.flags = Resource::FLAG_NONE,
		.data = this->keyValuesData,
	});
}

int8_t VTFWriter::getCompressionLevelForBake() const {
	return this->compressionLevel;
}

void VTFWriter::setCompressionLevelForBake(int8_t newCompressionLevel) {
	this->compressionLevel = newCompressionLevel;
}

bool VTFWriter::setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	uint8_t mips = mip;
	if (mips > 0 && this->mipCount == 0) {
		mips = ImageDimensions::getRecommendedMipCountForDims(this->width, this->height);
		if (mips <= mip) {
			return false;
		}
	}
	uint8_t faces = face == 1 ? 1 : ((this->majorVersion <= 7 && this->minorVersion < 5 && this->startFrame == 0xffff) ? 7 : 6);
	if (faces <= face) {
		return false;
	}
	this->regenerateImageData(this->format, this->width, this->height, std::max(this->mipCount, mips), std::max(this->frameCount, frame), std::max(this->getFaceCount(), faces), std::max(this->sliceCount, slice), filter);

	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, frame, this->frameCount, face, this->getFaceCount(), this->width, this->height, slice, this->sliceCount)) {
		std::vector<std::byte> image{imageData_.begin(), imageData_.end()};
		if (this->format != format_) {
			image = ImageConversion::convertImageDataToFormat(image, format_, this->format, this->width, this->height);
		}
		if (this->width != width_ || this->height != height_) {
			const bool srgbForResize = !ImageFormatDetails::large(this->format) && this->format != ImageFormat::RGBA16161616 && (this->flags & FLAG_SRGB);
			image = ImageConversion::resizeImageData(image, this->format, width_, this->width, height_, this->height, srgbForResize, filter);
		}
		std::memcpy(this->imageData.data() + offset, image.data(), image.size());
	}
	return true;
}

bool VTFWriter::setImage(const std::string& imagePath, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	const auto imageData_ = fs::readFileBuffer(imagePath);

	stbi_convert_iphone_png_to_rgb(true);
	int width_, height_, channels;
	if (stbi_is_hdr_from_memory(reinterpret_cast<const stbi_uc*>(imageData_.data()), static_cast<int>(imageData_.size()))) {
		std::unique_ptr<float, void(*)(void*)> stbImage{
			stbi_loadf_from_memory(reinterpret_cast<const stbi_uc*>(imageData_.data()), static_cast<int>(imageData_.size()), &width_, &height_, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return false;
		}

		ImageFormat imageFormat;
		switch (channels) {
			case 1: imageFormat = ImageFormat::R32F;          break;
			case 3: imageFormat = ImageFormat::RGB323232F;    break;
			case 4: imageFormat = ImageFormat::RGBA32323232F; break;
			default: return false;
		}

		return this->setImage({reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(imageFormat, width_, height_)}, imageFormat, width_, height_, filter, mip, frame, face, slice);
	} else if (stbi_is_16_bit_from_memory(reinterpret_cast<const stbi_uc*>(imageData_.data()), static_cast<int>(imageData_.size()))) {
		std::unique_ptr<stbi_us, void(*)(void*)> stbImage{
			stbi_load_16_from_memory(reinterpret_cast<const stbi_uc*>(imageData_.data()), static_cast<int>(imageData_.size()), &width_, &height_, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return false;
		}

		ImageFormat imageFormat;
		if (channels == 4) {
			imageFormat = ImageFormat::RGBA16161616;
		} else {
			return false;
		}

		return this->setImage({reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(imageFormat, width_, height_)}, imageFormat, width_, height_, filter, mip, frame, face, slice);
	} else {
		std::unique_ptr<stbi_uc, void(*)(void*)> stbImage{
			stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(imageData_.data()), static_cast<int>(imageData_.size()), &width_, &height_, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return false;
		}

		ImageFormat imageFormat;
		switch (channels) {
			case 1: imageFormat = ImageFormat::I8;       break;
			case 2: imageFormat = ImageFormat::UV88;     break;
			case 3: imageFormat = ImageFormat::RGB888;   break;
			case 4: imageFormat = ImageFormat::RGBA8888; break;
			default: return false;
		}

		return this->setImage({reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(imageFormat, width_, height_)}, imageFormat, width_, height_, filter, mip, frame, face, slice);
	}
}

std::vector<std::byte> VTFWriter::bake() {
	// todo(vtfpp): bake
	return {};
}

void VTFWriter::bake(const std::string& vtfPath) {
	fs::writeFileBuffer(vtfPath, this->bake());
}

void VTFWriter::regenerateImageData(ImageFormat newFormat, uint16_t newWidth, uint16_t newHeight, uint8_t newMipCount, uint16_t newFrameCount, uint8_t newFaceCount, uint16_t newSliceCount, ImageConversion::ResizeFilter filter) {
	if (this->format == newFormat && this->width == newWidth && this->height == newHeight && this->mipCount == newMipCount && this->frameCount == newFaceCount && this->getFaceCount() == newFaceCount && this->sliceCount == newSliceCount) {
		return;
	}

	const bool srgbForResize = !ImageFormatDetails::large(newFormat) && newFormat != ImageFormat::RGBA16161616 && (this->flags & FLAG_SRGB);

	std::vector<std::byte> newImageData(ImageFormatDetails::getDataLength(newFormat, newMipCount, newFrameCount, newFaceCount, newWidth, newHeight, newSliceCount));
	if (!this->imageData.empty()) {
		for (uint8_t i = newMipCount - 1; i >= 0; i--) {
			for (uint16_t j = 0; j < newFrameCount; j++) {
				for (uint8_t k = 0; k < newFaceCount; k++) {
					for (uint16_t l = 0; l < newSliceCount; l++) {
						if (i < this->mipCount && j < this->frameCount && k < this->getFaceCount() && l < this->sliceCount) {
							uint32_t oldOffset, oldLength;
							ImageFormatDetails::getDataPosition(oldOffset, oldLength, this->format, i, this->mipCount, j, this->frameCount, k, this->getFaceCount(), this->width, this->height, l, this->sliceCount);

							std::vector<std::byte> image{this->imageData.begin() + oldOffset, this->imageData.begin() + oldOffset + oldLength};
							if (this->format != newFormat) {
								image = ImageConversion::convertImageDataToFormat(image, this->format, newFormat, this->width, this->height);
							}
							if (this->width != newWidth || this->height != newHeight) {
								image = ImageConversion::resizeImageData(image, newFormat, this->width, newWidth, this->height, newHeight, srgbForResize, filter);
							}

							uint32_t newOffset, newLength;
							ImageFormatDetails::getDataPosition(newOffset, newLength, newFormat, i, newMipCount, j, newFrameCount, k, newFaceCount, newWidth, newHeight, l, newSliceCount);
							std::memcpy(newImageData.data() + newOffset, image.data(), image.size());
						}
					}
				}
			}
		}
	}

	this->format = newFormat;
	this->width = newWidth;
	this->height = newHeight;
	this->mipCount = newMipCount;
	this->frameCount = newFrameCount;
	if (newFaceCount > 1) {
		this->flags |= FLAG_ENVMAP;
		if (this->majorVersion <= 7 && this->minorVersion < 5 && newFaceCount == 7) {
			this->startFrame = 0xffff;
		}
	} else {
		this->flags &= ~FLAG_ENVMAP;
	}
	this->sliceCount = newSliceCount;

	this->imageData = std::move(newImageData);

	bool foundResource = false;
	for (auto& resource : this->resources) {
		if (resource.type == Resource::TYPE_IMAGE_DATA) {
			resource.data = this->imageData;
			foundResource = true;
			break;
		}
	}
	if (!foundResource) {
		this->resources.push_back({
			.type = Resource::TYPE_IMAGE_DATA,
			.flags = Resource::FLAG_NONE,
			.data = this->imageData,
		});
	}
}
