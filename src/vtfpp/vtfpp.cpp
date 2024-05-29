#include <vtfpp/vtfpp.h>

#include <BufferStream.h>

#include <vtfpp/ImageConversion.h>

using namespace sourcepp;
using namespace vtfpp;

Resource::ConvertedData Resource::convertData() const {
	switch (this->type) {
		case TYPE_CRC:
		case TYPE_EXTENDED_FLAGS:
			if (this->data.size() != 4) {
				return {};
			}
			return *reinterpret_cast<const uint32_t*>(this->data.data());
		case TYPE_LOD_CONTROL_INFO:
			if (this->data.size() != 4) {
				return {};
			}
			return std::make_pair(
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 0),
					*(reinterpret_cast<const uint8_t*>(this->data.data()) + 1)
			);
		case TYPE_KEYVALUES_DATA:
			if (this->data.size() <= 4) {
				return std::string_view{""};
			}
			return std::string_view{reinterpret_cast<const char*>(this->data.data() + 4), *reinterpret_cast<const uint32_t*>(this->data.data()) + 1};
		default:
			break;
	}
	return {};
}

VTF::VTF(const std::byte* vtfData, std::size_t vtfSize, const VTFOptions& options) {
	BufferStream stream{vtfData, vtfSize};

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

	if (options.parseHeaderOnly) {
		this->opened = true;
		return;
	}

	if (this->minorVersion >= 3) {
		stream.skip(3);
		auto resourceCount = stream.read<uint32_t>();
		stream.skip(8);

		int lastResourceIndex = -1;
		for (int i = 0; i < resourceCount; i++) {
			auto& resource = this->resources.emplace_back(new Resource{});

			stream
				.read(resource->type)
				.skip(2)
				.read(resource->flags);
			resource->data = stream.read_bytes(4);

			if (!(resource->flags & Resource::FLAG_NO_DATA)) {
				if (lastResourceIndex >= 0) {
					auto lastOffset = *reinterpret_cast<uint32_t*>(this->resources[lastResourceIndex]->data.data());
					auto currentOffset = *reinterpret_cast<uint32_t*>(resource->data.data());

					auto curPos = stream.tell();
					stream.seek(lastOffset);
					this->resources[lastResourceIndex]->data = stream.read_bytes(currentOffset - lastOffset);
					stream.seek(curPos);
				}
				lastResourceIndex = static_cast<int>(this->resources.size() - 1);
			}
		}
		if (lastResourceIndex >= 0) {
			auto offset = *reinterpret_cast<uint32_t*>(this->resources[lastResourceIndex]->data.data());
			auto curPos = stream.tell();
			stream.seek(offset);
			this->resources[lastResourceIndex]->data = stream.read_bytes(stream.size() - offset);
			stream.seek(curPos);
		}

		// Null-terminate KeyValues data so std::string_view works
		for (auto& resource : this->resources) {
			if (resource->type == Resource::TYPE_KEYVALUES_DATA) {
				resource->data.push_back(std::byte{'\0'});
			}
		}

		this->opened = stream.tell() == headerLength;
	} else {
		while (stream.tell() % 16 != 0) {
			stream.skip(1);
		}
		this->opened = stream.tell() == headerLength;

		if (this->thumbnailWidth > 0 && this->thumbnailHeight > 0) {
			auto thumbnailLength = ImageFormatDetails::getDataLength(this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight);
			this->resources.emplace_back(new Resource{Resource::TYPE_THUMBNAIL_DATA, Resource::FLAG_NONE, stream.read_bytes(thumbnailLength)});
		}
		if (this->width > 0 && this->height > 0) {
			auto imageLength = ImageFormatDetails::getDataLength(this->format, this->mipCount, this->frameCount, this->getFaceCount(), this->width, this->height, this->sliceCount);
			this->resources.emplace_back(new Resource{Resource::TYPE_IMAGE_DATA, Resource::FLAG_NONE, stream.read_bytes(imageLength)});
		}
	}
}

VTF::VTF(const unsigned char* vtfData, std::size_t vtfSize, const VTFOptions& options)
		: VTF(reinterpret_cast<const std::byte*>(vtfData), vtfSize, options) {}

VTF::VTF(const std::vector<std::byte>& vtfData, const VTFOptions& options)
		: VTF(vtfData.data(), vtfData.size(), options) {}

VTF::VTF(const std::vector<unsigned char>& vtfData, const VTFOptions& options)
		: VTF(vtfData.data(), vtfData.size(), options) {}

VTF::operator bool() const {
	return this->opened;
}

uint32_t VTF::getMajorVersion() const {
	return this->majorVersion;
}

uint32_t VTF::getMinorVersion() const {
	return this->minorVersion;
}

uint16_t VTF::getWidth() const {
	return this->width;
}

uint16_t VTF::getHeight() const {
	return this->height;
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

Vec3f VTF::getReflectivity() const {
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

const std::vector<std::unique_ptr<Resource>>& VTF::getResources() const {
	return this->resources;
}

const Resource* VTF::getResource(Resource::Type type) const {
	for (const auto& resource : this->resources) {
		if (resource->type == type) {
			return resource.get();
		}
	}
	return nullptr;
}

std::span<const std::byte> VTF::getImageData(uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
	if (auto imageResource = this->getResource(Resource::TYPE_IMAGE_DATA)) {
		uint32_t offset, length;
		if (!ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, frame, this->frameCount, face, this->getFaceCount(), this->width, this->height, slice, this->sliceCount)) {
			return {};
		}
		return std::span<const std::byte>{imageResource->data}.subspan(offset, length);
	}
	return {};
}

std::vector<std::byte> VTF::getImageDataAs(ImageFormat newFormat, uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
	auto rawImageData = this->getImageData(mip, frame, face, slice);
	if (rawImageData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFormat(rawImageData, this->format, newFormat, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
}

std::vector<std::byte> VTF::getImageDataAsRGBA8888(uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
	return this->getImageDataAs(ImageFormat::RGBA8888, mip, frame, face, slice);
}

std::vector<std::byte> VTF::convertImageDataToFile(uint8_t mip, uint16_t frame, uint16_t face, uint16_t slice) const {
	auto rawImageData = this->getImageData(mip, frame, face, slice);
	if (rawImageData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFile(rawImageData, this->format, ImageDimensions::getMipDim(mip, this->width), ImageDimensions::getMipDim(mip, this->height));
}

std::span<const std::byte> VTF::getThumbnailData() const {
	if (auto thumbnailResource = this->getResource(Resource::TYPE_THUMBNAIL_DATA)) {
		return {thumbnailResource->data};
	}
	return {};
}

std::vector<std::byte> VTF::getThumbnailDataAs(ImageFormat newFormat) const {
	auto rawThumbnailData = this->getThumbnailData();
	if (rawThumbnailData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFormat(rawThumbnailData, this->thumbnailFormat, newFormat, this->thumbnailWidth, this->thumbnailHeight);
}

std::vector<std::byte> VTF::getThumbnailDataAsRGBA8888() const {
	return this->getThumbnailDataAs(ImageFormat::RGBA8888);
}

std::vector<std::byte> VTF::convertThumbnailDataToFile() const {
	auto rawThumbnailData = this->getThumbnailData();
	if (rawThumbnailData.empty()) {
		return {};
	}
	return ImageConversion::convertImageDataToFile(rawThumbnailData, this->thumbnailFormat, this->thumbnailWidth, this->thumbnailHeight);
}
