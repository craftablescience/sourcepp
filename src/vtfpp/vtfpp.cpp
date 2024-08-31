#include <vtfpp/vtfpp.h>

#include <BufferStream.h>
#include <miniz.h>

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
			return std::string(reinterpret_cast<const char*>(this->data.data() + 4), *reinterpret_cast<const uint32_t*>(this->data.data()));
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
			resource.type = static_cast<Resource::Type>(typeAndFlags & 0xFFFFFF); // last 3 bytes
			resource.flags = static_cast<Resource::Flags>(typeAndFlags >> 24); // first byte
			resource.data = stream.read_span<std::byte>(4);

			if (!(resource.flags & Resource::FLAG_NO_DATA)) {
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
		while (stream.tell() % 16 != 0) {
			stream.skip();
		}
		this->opened = stream.tell() == headerLength;

		this->resources.reserve(2);

		if (this->thumbnailWidth > 0 && this->thumbnailHeight > 0) {
			auto thumbnailLength = ImageFormatDetails::getDataLength(this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight);
			this->resources.push_back({Resource::TYPE_THUMBNAIL_DATA, Resource::FLAG_NONE, stream.read_span<std::byte>(thumbnailLength)});
		}
		if (this->width > 0 && this->height > 0) {
			auto imageLength = ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount);
			this->resources.push_back({Resource::TYPE_IMAGE_DATA, Resource::FLAG_NONE, stream.read_span<std::byte>(imageLength)});
		}
	}
}

VTF::VTF(std::span<const std::byte> vtfData, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{vtfData.begin(), vtfData.end()}, parseHeaderOnly) {}

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

uint16_t VTF::getFaceCount() const {
	return (this->flags & VTF::FLAG_ENVMAP) ? (this->minorVersion < 5 ? 7 : 6) : 1;
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

std::span<const std::byte> VTF::getImageDataRaw(uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
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
				for (uint16_t k = 0; k < this->getFaceCount(); k++) {
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

std::vector<std::byte> VTF::getImageDataAs(ImageFormat newFormat, uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
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

std::vector<std::byte> VTF::getImageDataAsRGBA8888(uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
	return this->getImageDataAs(ImageFormat::RGBA8888, mip, frame, face, slice);
}

std::vector<std::byte> VTF::convertAndSaveImageDataToFile(uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
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
