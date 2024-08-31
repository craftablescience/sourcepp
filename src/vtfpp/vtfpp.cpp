#include <vtfpp/vtfpp.h>

#include <BufferStream.h>
#include <miniz.h>

#include <vtfpp/ImageConversion.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace sourcepp;
using namespace vtfpp;

/*
 * This function is modified from Godot Engine code, licensed under the MIT License.
 * Copyright (c) 2014-present Godot Engine contributors.
 * Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.
 * https://github.com/godotengine/godot/blob/99ff024f78f65ba0bc54fb409cfeca43ba2008fe/core/io/pck_packer.cpp#L39
 */
std::size_t getPadding(int alignment, int n) {
    if (const int rest = n % alignment; rest > 0) {
        return alignment - rest;
    }
    return 0;
}

bool isPowerOfTwo(std::integral auto n)
{
    return (n & (n - 1)) == 0;
}

std::integral auto nextPowerOfTwo(std::integral auto n)
{
    return std::bit_ceil(n);
}

std::integral auto lastPowerOfTwo(std::integral auto n)
{
    return std::bit_floor(n);
}

std::integral auto nearestPowerOfTwo(std::integral auto n)
{
    if(isPowerOfTwo(n))
        return n;

    std::integral auto next = nextPowerOfTwo(n);
    std::integral auto last = lastPowerOfTwo(n);

    return (n - last) < (next - n) ? last : next;
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

VTF::VTF(const std::vector<std::byte>& vtfData, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{vtfData.begin(), vtfData.end()}, parseHeaderOnly) {}

VTF::VTF(const std::vector<unsigned char>& vtfData, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{reinterpret_cast<const std::byte*>(vtfData.data()), reinterpret_cast<const std::byte*>(vtfData.data() + vtfData.size())}, parseHeaderOnly) {}

VTF::VTF(const std::byte* vtfData, std::size_t vtfSize, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{vtfData, vtfData + vtfSize}, parseHeaderOnly) {}

VTF::VTF(const unsigned char* vtfData, std::size_t vtfSize, bool parseHeaderOnly)
		: VTF(std::vector<std::byte>{reinterpret_cast<const std::byte*>(vtfData), reinterpret_cast<const std::byte*>(vtfData + vtfSize)}, parseHeaderOnly) {}

VTF::operator bool() const {
	return this->opened;
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
		for (int i = this->mipCount - 1; i >= 0; i--) {
			for (int j = 0; j < this->frameCount; j++) {
				for (int k = 0; k < this->getFaceCount(); k++) {
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

const std::vector<std::byte>& VTF::saveVTFToFile() const {
    return this->data;
}

VTFBuilder::VTFBuilder(const std::vector<std::byte>& data)
{
    this->builderData.imageData = data;
}

uint8_t VTFBuilder::computeMipmapCount(uint16_t width, uint16_t height, uint16_t slices)
{
    return std::bit_width(std::max(width,height));
}

std::unique_ptr<VTF> VTFBuilder::build() {
    auto creationOptions = this->builderData;

    if(creationOptions.majorVersion > 7 || creationOptions.minorVersion > 6)
        return nullptr;

    if(creationOptions.width == 0 || creationOptions.height == 0)
        return nullptr;

    if(ImageFormatDetails::getDataLength(creationOptions.inputImageFormat,1,creationOptions.frames,creationOptions.getFaceCount(),creationOptions.width,creationOptions.height,creationOptions.slices) > builderData.imageData.size())
        return nullptr;

    if(ImageFormatDetails::getDataLength(creationOptions.inputImageFormat,1,creationOptions.frames,creationOptions.getFaceCount(),creationOptions.width,creationOptions.height,creationOptions.slices) < builderData.imageData.size())
        builderData.imageData.resize(ImageFormatDetails::getDataLength(creationOptions.inputImageFormat,1,creationOptions.frames,creationOptions.getFaceCount(),creationOptions.width,creationOptions.height,creationOptions.slices));

    uint16_t width = creationOptions.width;
    uint16_t height = creationOptions.height;
    uint16_t frames = creationOptions.frames;
    uint16_t faces = creationOptions.getFaceCount();
    uint16_t slices = creationOptions.slices;

    if(ImageFormatDetails::alphaBits(creationOptions.destinationImageFormat) == 1)
        creationOptions.flags |= VTF::FLAG_ONE_BIT_ALPHA;

    if(ImageFormatDetails::alphaBits(creationOptions.destinationImageFormat) > 1)
        creationOptions.flags |= VTF::FLAG_MULTI_BIT_ALPHA;

    if(faces != 1)
        creationOptions.flags |= VTF::FLAG_ENVMAP;

    if( faces == 6 )
        creationOptions.startFrame = 0xffff;

    if(!isPowerOfTwo(width) || !isPowerOfTwo(height) || creationOptions.shouldResizeImage)
    {
        //Default is RESIZE_SET;
        uint16_t resizeWidth = creationOptions.resizeWidth;
        uint16_t resizeHeight = creationOptions.resizeHeight;

        if(creationOptions.resizeMethod == RESIZE_BIGGEST_POWER2) {
            resizeWidth = nextPowerOfTwo(width);
            resizeHeight = nextPowerOfTwo(height);
        }

        if(creationOptions.resizeMethod == RESIZE_SMALLEST_POWER2) {
            resizeWidth = lastPowerOfTwo(width);
            resizeHeight = lastPowerOfTwo(height);
        }

        if(creationOptions.resizeMethod == RESIZE_NEAREST_POWER2) {
            resizeWidth = nearestPowerOfTwo(width);
            resizeHeight = nearestPowerOfTwo(height);
        }

        std::vector<std::byte> resizedImageData{};
        resizedImageData.resize(ImageFormatDetails::getDataLength(creationOptions.inputImageFormat, 1,frames, faces, resizeWidth,resizeHeight, slices));
        for(int i = 0; i < frames; i++)
            for(int j = 0; j < faces; j++)
                for(int k = 0; k < slices; k++)
                {
                    uint32_t offset;
                    uint32_t length;
                    uint32_t resizeOffset;
                    uint32_t resizeLength;
                    (void*) ImageFormatDetails::getDataPosition(offset,length,creationOptions.inputImageFormat,0,1,i,frames,j,faces,width,height,k,slices);
                    (void*) ImageFormatDetails::getDataPosition(resizeOffset,resizeLength,creationOptions.inputImageFormat,0,1,i,frames,j,faces,resizeWidth,resizeHeight,k,slices);
                    std::vector<std::byte> singleImage{};
                    singleImage.resize(length);
                    std::memcpy(singleImage.data(),creationOptions.imageData.data() + offset,length);
                    singleImage = resizeImageData(singleImage,creationOptions.inputImageFormat,width,height,resizeWidth,resizeHeight,creationOptions.resizeFilter,creationOptions.flags & VTF::FLAG_SRGB);
                    std::memcpy(resizedImageData.data() + resizeOffset,singleImage.data(),resizeLength);
                }
        creationOptions.imageData = resizedImageData;
        creationOptions.width = width = resizeWidth;
        creationOptions.height = height = resizeHeight;
    }

    if(creationOptions.computeReflectivity)
    {
        auto tmpData = ImageConversion::convertImageDataToFormat(creationOptions.imageData, creationOptions.inputImageFormat, ImageFormat::RGB888,width, height);
        auto singlePixel = resizeImageData(tmpData,ImageFormat::RGB888,width,height,1,1,creationOptions.resizeFilter,creationOptions.flags & VTF::FLAG_SRGB);
        creationOptions.reflectivity[0] = (1.0f / static_cast<uint8_t >(singlePixel[0]));
        creationOptions.reflectivity[1] = (1.0f / static_cast<uint8_t >(singlePixel[1]));
        creationOptions.reflectivity[2] = (1.0f / static_cast<uint8_t >(singlePixel[2]));
    }

    if(creationOptions.generateMipmaps)
    {
        creationOptions.imageData = computeMipmaps(creationOptions.imageData, creationOptions.inputImageFormat,width,height,frames,faces,slices,creationOptions.mipmapFilter,creationOptions.flags & VTF::FLAG_SRGB);
    } else
        creationOptions.flags |= VTF::FLAG_NO_MIP | VTF::FLAG_NO_LOD;

    if(creationOptions.inputImageFormat != creationOptions.destinationImageFormat)
    {
        uint8_t mipCount = creationOptions.generateMipmaps ? computeMipmapCount(width,height,slices) : 1;
        creationOptions.resources[TYPE_IMAGE_DATA] = ImageConversion::convertImageDataToFormat(creationOptions.imageData,creationOptions.inputImageFormat, creationOptions.destinationImageFormat, width, height,frames,faces,mipCount,slices);
    } else
        creationOptions.resources[TYPE_IMAGE_DATA] = creationOptions.imageData;

    if(creationOptions.generateThumbnail)
    {
        std::vector<std::byte> tmp = resizeImageData(creationOptions.imageData,creationOptions.destinationImageFormat,width,height,16,16,creationOptions.thumbnailFilter,creationOptions.flags & VTF::FLAG_SRGB);
        creationOptions.resources[TYPE_THUMBNAIL_DATA] = ImageConversion::convertImageDataToFormat(tmp, creationOptions.destinationImageFormat,ImageFormat::DXT1, 16, 16);
    }

    std::vector<std::byte> rawVTFData;
    BufferStream vtfFile{rawVTFData,true};

    vtfFile.write<uint32_t>(VTF_SIGNATURE)
    .write<uint32_t>(creationOptions.majorVersion)
    .write<uint32_t>(creationOptions.minorVersion)
    .write<uint32_t>(0)
    .write<uint16_t>(creationOptions.width)
    .write<uint16_t>(creationOptions.height)
    .write<uint32_t>(creationOptions.flags)
    .write<uint16_t>(creationOptions.frames)
    .write<uint16_t>(creationOptions.startFrame)
    .write<uint32_t>(0) //padding;
    .write<float>(creationOptions.reflectivity[0])
    .write<float>(creationOptions.reflectivity[1])
    .write<float>(creationOptions.reflectivity[2])
    .write<uint32_t>(0) //padding;
    .write<float>(creationOptions.bumpScale)
    .write<ImageFormat>(creationOptions.destinationImageFormat)
    .write<uint8_t>(creationOptions.generateMipmaps ? computeMipmapCount(width,height,slices) : 1)
    .write<ImageFormat>(ImageFormat::DXT1)
    .write<uint8_t>(16)
    .write<uint8_t>(16);
    if(creationOptions.minorVersion < 2)
        goto VTFEOH;
    vtfFile.write<uint16_t>(creationOptions.slices);
    if(creationOptions.minorVersion < 3)
        goto VTFEOH;
    vtfFile.write<uint16_t>(0)//padding
    .write<uint8_t>(0)//padding
    .write<uint32_t>(creationOptions.resources.size())
    .write<uint64_t>(0);//padding

    VTFEOH:
    auto padding = getPadding(16, vtfFile.size());
    for (int i = 0; i < padding; i++)
        vtfFile.write<uint8_t>(0);//alignment

    auto headerEnd = vtfFile.tell();
    vtfFile
            .seek(sizeof(uint32_t) * 3)
            .write<uint32_t>(headerEnd + (creationOptions.minorVersion > 2 ? creationOptions.resources.size() * sizeof(uint64_t) : 0))
            .seek(headerEnd);
    if(creationOptions.minorVersion > 2)
    {
        //NOTE: The VTF layout for resources is rather specific.
        //-Lowres image data.
        //-Every other resource that is NOT the high res image.
        //-High res image.
        //Therefore we must separate them out of the resource pile and insert them accordingly.

        uint32_t calculateOffset = headerEnd + (sizeof(uint64_t) * creationOptions.resources.size());

        std::vector<std::byte> image = creationOptions.resources[TYPE_IMAGE_DATA];
        creationOptions.resources.erase(TYPE_IMAGE_DATA);

        //Lowres first.
        std::vector<std::byte> lowResmage;
        if(creationOptions.generateThumbnail) {
            lowResmage = creationOptions.resources[TYPE_THUMBNAIL_DATA];
            creationOptions.resources.erase(TYPE_THUMBNAIL_DATA);

            vtfFile.write<uint32_t>(TYPE_THUMBNAIL_DATA)
                    .write<uint32_t>(calculateOffset);
            calculateOffset += lowResmage.size();
        }

        //Everything else.
        for (const auto &[type, resource]: creationOptions.resources)
        {
            vtfFile.write<uint32_t>(type)
            .write<uint32_t>(calculateOffset);
            calculateOffset += resource.size();
        }

        //Highres image
        vtfFile.write<uint32_t>(TYPE_IMAGE_DATA).write<uint32_t>(calculateOffset );

        //Insert lowres image
        if(creationOptions.generateThumbnail)
            vtfFile.write(lowResmage);

        //insert everything else.
        for (const auto &[type, resource]: creationOptions.resources)
        {
            vtfFile.write(resource);
        }

        vtfFile.write(image);


    } else {
        if(creationOptions.generateThumbnail)
            vtfFile.write(creationOptions.resources[TYPE_THUMBNAIL_DATA]);
        vtfFile.write(creationOptions.resources[TYPE_IMAGE_DATA]);
    }
    rawVTFData.resize(vtfFile.size());
    std::unique_ptr<VTF> vtf = std::make_unique<VTF>(rawVTFData);

    return std::move(vtf);

}

std::vector<std::byte>
VTFBuilder::computeMipmaps(const std::vector<std::byte> &data, ImageFormat format, uint16_t width, uint16_t height,
                           uint16_t frames, uint16_t faces, uint16_t slices, ResizeFilter resizeFilter, bool srgb)
{

    uint8_t mipCount = computeMipmapCount(width,height,slices);
    if(!ImageFormatDetails::large(format)) {

        std::vector<std::byte> tmpData = ImageConversion::convertImageDataToFormat(data,format, ImageFormat::RGBA8888, width, height,frames,faces,1,slices);
        std::vector<std::byte> mipData{};
        mipData.resize(ImageFormatDetails::getDataLength(ImageFormat::RGBA8888, mipCount, frames, faces, width, height, slices));
        for (int i = 0; i < frames; i++)
            for (int j = 0; j < faces; j++)
                for (int k = 0; k < slices; k++)
                    for (int l = 0,prev = -1; l < mipCount; l++, prev++) {

                        bool belZir = prev == -1;
                        if(belZir)
                            prev = 0;

                        uint32_t mipOffset;
                        uint32_t mipLength;
                        uint32_t offset;
                        uint32_t length;

                        uint16_t mipWidth = ImageDimensions::getMipDim(l,width);
                        uint16_t mipHeight = ImageDimensions::getMipDim(l, height);
                        uint16_t prevMipWidth = ImageDimensions::getMipDim( prev, width);
                        uint16_t prevMipHeight = ImageDimensions::getMipDim(prev, height);

                        (void*) ImageFormatDetails::getDataPosition(mipOffset, mipLength, ImageFormat::RGBA8888, l, mipCount, i,
                                                                     frames, j, faces, width, height, k, slices);

                        (void*) ImageFormatDetails::getDataPosition(offset, length, ImageFormat::RGBA8888, prev, belZir ? 1 : mipCount, i, frames, j, faces,
                                                                     width, height, k, slices);




                        auto tmpResizedData = resizeImageData({(belZir ? tmpData : mipData).data() + offset,(belZir ? tmpData : mipData).data() + offset + length}, ImageFormat::RGBA8888, prevMipWidth, prevMipHeight, mipWidth, mipHeight, resizeFilter, srgb);
                        std::memcpy(mipData.data() + mipOffset, tmpResizedData.data(), mipLength);

                        if(belZir)
                            prev = -1;
                    }
        return ImageConversion::convertImageDataToFormat(mipData,ImageFormat::RGBA8888, format, width, height,frames,faces,mipCount,slices);
    }
    else if(format == ImageFormat::RGBA16161616)
    {
        std::vector<std::byte> mipData{};
        mipData.resize(ImageFormatDetails::getDataLength(ImageFormat::RGBA16161616, mipCount, frames, faces, width, height, slices));
        for (int i = 0; i < frames; i++)
            for (int j = 0; j < faces; j++)
                for (int k = 0; k < slices; k++)
                    for (int l = 0,prev = -1; l < mipCount; l++, prev++) {

                        bool belZir = prev == -1;
                        if(belZir)
                            prev = 0;

                        uint32_t mipOffset;
                        uint32_t mipLength;
                        uint32_t offset;
                        uint32_t length;

                        uint16_t mipWidth = ImageDimensions::getMipDim(l,width);
                        uint16_t mipheight = ImageDimensions::getMipDim(l,height);
                        uint16_t prevMipWidth = ImageDimensions::getMipDim( prev, width);
                        uint16_t prevMipHeight = ImageDimensions::getMipDim(prev, height);


                        (void*) ImageFormatDetails::getDataPosition(mipOffset, mipLength, ImageFormat::RGBA16161616, l, mipCount, i,
                                                                     frames, j, faces, width, height, k, slices);
                        (void*) ImageFormatDetails::getDataPosition(offset, length, ImageFormat::RGBA16161616, prev, belZir ? 1 : mipCount, i, frames, j, faces,
                                                                     width, height, k, slices);

                        auto tmpResizedData = resizeImageData({(belZir ? data : mipData).data() + offset,(belZir ? data : mipData).data() + offset + length},ImageFormat::RGBA16161616,prevMipWidth,prevMipHeight,mipWidth,mipheight,resizeFilter,srgb);
                        std::memcpy(mipData.data() + mipOffset, tmpResizedData.data(), mipLength);

                        if(belZir)
                            prev = -1;
                    }
        return mipData;
    } else {
        std::vector<std::byte> tmpData = ImageConversion::convertImageDataToFormat(data,format, ImageFormat::RGBA8888, width, height,frames,faces,mipCount,slices);
        std::vector<std::byte> mipData{};
        mipData.resize(ImageFormatDetails::getDataLength(ImageFormat::RGBA32323232F, mipCount, frames, faces, width, height, slices));
        for (int i = 0; i < frames; i++)
            for (int j = 0; j < faces; j++)
                for (int k = 0; k < slices; k++)
                    for (int l = 0,prev = -1; l < mipCount; l++, prev++) {

                        bool belZir = prev == -1;
                        if(belZir)
                            prev = 0;

                        uint32_t mipOffset;
                        uint32_t mipLength;
                        uint32_t offset;
                        uint32_t length;

                        uint16_t mipWidth = ImageDimensions::getMipDim(l,width);
                        uint16_t mipheight = ImageDimensions::getMipDim(l,height);
                        uint16_t prevMipWidth = ImageDimensions::getMipDim( prev, width);
                        uint16_t prevMipHeight = ImageDimensions::getMipDim(prev, height);

                        (void*) ImageFormatDetails::getDataPosition(mipOffset, mipLength, ImageFormat::RGBA32323232F, l, mipCount, i,
                                                                     frames, j, faces, width, height, k, slices);
                        (void*) ImageFormatDetails::getDataPosition(offset, length, ImageFormat::RGBA32323232F, prev, belZir ? 1 : mipCount, i, frames, j, faces,
                                                                     width, height, k, slices);

                        auto tmpResizedData = resizeImageData({(belZir ? tmpData : mipData).data() + offset,(belZir ? tmpData : mipData).data() + offset + length},ImageFormat::RGBA32323232F,prevMipWidth,prevMipHeight,mipWidth,mipheight,resizeFilter,srgb);
                        std::memcpy(mipData.data() + mipOffset, tmpResizedData.data(), mipLength);

                        if(belZir)
                            prev = -1;
                    }
        return ImageConversion::convertImageDataToFormat(mipData,ImageFormat::RGBA32323232F, format, width, height,frames,faces,mipCount,slices);
    }
}

VTFBuilder &VTFBuilder::removeFlags(VTF::Flags flag) {
    if(flag & VTF::FLAG_ONE_BIT_ALPHA || flag & VTF::FLAG_MULTI_BIT_ALPHA)
    {
        std::cout << "Invalid flag. Alpha bit cannot be removed by user." << std::endl;
        return *this;
    }

    this->builderData.flags &= ~flag;
    return *this;
}

VTFBuilder &VTFBuilder::setBumpScale(float scale) {
    this->builderData.bumpScale = scale;
    return *this;
}

VTFBuilder &VTFBuilder::addFlags(VTF::Flags flag) {
    if(flag & VTF::FLAG_ONE_BIT_ALPHA || flag & VTF::FLAG_MULTI_BIT_ALPHA)
    {
        std::cout << "Invalid flag. Alpha bit cannot be set by user." << std::endl;
        return *this;
    }

    this->builderData.flags |= flag;
    return *this;
}

VTFBuilder &VTFBuilder::applyAuxCompression(int8_t compression) {
    this->builderData.applyAuxCompression = false;
    this->builderData.auxCompressionLevel = compression;
    return *this;
}

VTFBuilder &VTFBuilder::removeResource(VTFBuilder::ResourceType resourceType) {
    if(resourceType == TYPE_IMAGE_DATA || resourceType == TYPE_THUMBNAIL_DATA)
    {
        std::cout << "Image data resources is used internally, cannot be removed by user." << std::endl;
        return *this;
    }

    if(!this->builderData.resources.contains(resourceType))
        this->builderData.resources.erase(resourceType);
    return *this;
}

VTFBuilder &VTFBuilder::addResource(VTFBuilder::ResourceType resourceType, const std::vector<std::byte> &data) {
    if(resourceType == TYPE_IMAGE_DATA || resourceType == TYPE_THUMBNAIL_DATA)
    {
        std::cout << "Image data resources is set internally, cannot be set by user." << std::endl;
        return *this;
    }

    this->builderData.resources[resourceType] = data;

    return *this;
}

VTFBuilder &VTFBuilder::applyGammaCorrection(float correction) {
    this->builderData.applyGammaCorrection = true;
    this->builderData.gammaCorrection = correction;
    return *this;
}

VTFBuilder &VTFBuilder::resizeClamp(uint16_t resizeWidth, uint16_t resizeHeight) {
    this->builderData.shouldResizeToClamp = true;
    this->builderData.uiResizeClampWidth = resizeWidth;
    this->builderData.uiResizeClampHeight = resizeHeight;
    return *this;
}

VTFBuilder &VTFBuilder::setResizeFilter(ResizeFilter filter) {
    this->builderData.resizeFilter = filter;
    return *this;
}

VTFBuilder &VTFBuilder::setResizeMethod(VTFBuilder::ResizeMethod resizeMethod) {
    this->builderData.resizeMethod = resizeMethod;
    return *this;
}

VTFBuilder &VTFBuilder::generateThumbnail(ResizeFilter mipmapFilter) {
    this->builderData.generateThumbnail = true;
    this->builderData.thumbnailFilter = mipmapFilter;
    return *this;
}

VTFBuilder &VTFBuilder::generateMipmaps(ResizeFilter mipmapFilter) {
    this->builderData.generateMipmaps = true;
    this->builderData.mipmapFilter = mipmapFilter;
    return *this;
}

VTFBuilder &VTFBuilder::resizeImage(uint16_t resizeWidth, uint16_t resizeHeight, ResizeFilter resizeFilter) {
    this->builderData.shouldResizeImage = true;
    this->builderData.resizeWidth = resizeWidth;
    this->builderData.resizeHeight = resizeHeight;
    this->builderData.resizeFilter = resizeFilter;
    this->builderData.resizeMethod = ResizeMethod::RESIZE_SET;
    return *this;
}

VTFBuilder &VTFBuilder::setReflectivity(math::Vec3f reflectivity) {
    this->builderData.reflectivity = reflectivity;
    return *this;
}

VTFBuilder &VTFBuilder::computeReflectivity() {
    this->builderData.computeReflectivity = true;
    return *this;
}

VTFBuilder &VTFBuilder::setStartFrame(uint16_t frame) {
    this->builderData.startFrame = frame;
    return *this;
}

VTFBuilder &VTFBuilder::setSliceCount(uint16_t count) {
    this->builderData.slices = count;
    return *this;
}

VTFBuilder &VTFBuilder::setFrameCount(uint16_t count) {
    this->builderData.frames = count;
    return *this;
}

VTFBuilder &VTFBuilder::setVersion(uint8_t majorVersion, uint8_t minorVersion) {
    this->builderData.majorVersion = majorVersion;
    this->builderData.minorVersion = minorVersion;
    return *this;
}

VTFBuilder &VTFBuilder::setVTFFormat(ImageFormat format) {
    this->builderData.destinationImageFormat = format;
    return *this;
}

VTFBuilder &VTFBuilder::setInputDataFormat(ImageFormat format) {
    this->builderData.inputImageFormat = format;
    return *this;
}

VTFBuilder &VTFBuilder::setDimensions(uint16_t width, uint16_t height) {
    this->builderData.width = width;
    this->builderData.height = height;
    return *this;
}

std::vector<std::byte>
VTFBuilder::resizeImageData(const std::vector<std::byte> &imageData, ImageFormat format, uint16_t width,
                            uint16_t height, uint16_t resizeWidth, uint16_t resizeHeight, ResizeFilter resizeFilter,
                            bool isSRGBGColorSpace) {
    if(!ImageFormatDetails::large(format) && format != ImageFormat::RGBA16161616)
    {
        std::vector<std::byte> convertedData = ImageConversion::convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888,width, height);
        std::vector<std::byte> resizedImageData;
        resizedImageData.resize(ImageFormatDetails::getDataLength(ImageFormat::RGBA8888, resizeWidth,resizeHeight));
        stbir_resize_uint8_generic(reinterpret_cast<const unsigned char *>(convertedData.data()), width, height, 0,
                                   reinterpret_cast<unsigned char *>(resizedImageData.data()), resizeWidth, resizeHeight, 0, 4, 3, 0, STBIR_EDGE_CLAMP, static_cast<stbir_filter>(resizeFilter), isSRGBGColorSpace ? STBIR_COLORSPACE_SRGB : STBIR_COLORSPACE_LINEAR, nullptr);
        return ImageConversion::convertImageDataToFormat(resizedImageData, ImageFormat::RGBA8888,format,resizeWidth, resizeHeight);
    } else if(format == ImageFormat::RGBA16161616) //Because it's exists.
    {
        std::vector<std::byte> resizedImageData;
        resizedImageData.resize(ImageFormatDetails::getDataLength(ImageFormat::RGBA16161616, resizeWidth,resizeHeight));
        stbir_resize_uint16_generic(reinterpret_cast<const stbir_uint16 *>(imageData.data()), width, height, 0,
                                    reinterpret_cast<stbir_uint16 *>(resizedImageData.data()), resizeWidth, resizeHeight, 0, 4, 3, 0, STBIR_EDGE_CLAMP, static_cast<stbir_filter>(resizeFilter), isSRGBGColorSpace ? STBIR_COLORSPACE_SRGB : STBIR_COLORSPACE_LINEAR, nullptr);
        return resizedImageData;
    }
    else
    {
        std::vector<std::byte> convertedData = ImageConversion::convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F,width, height);
        std::vector<std::byte> resizedImageData;
        resizedImageData.resize(ImageFormatDetails::getDataLength(ImageFormat::RGBA32323232F, resizeWidth,resizeHeight));
        stbir_resize_float_generic(reinterpret_cast<const float *>(convertedData.data()), width, height, 0,
                                   reinterpret_cast<float *>(resizedImageData.data()), resizeWidth, resizeHeight, 0, 4, 3, 0, STBIR_EDGE_CLAMP, static_cast<stbir_filter>(resizeFilter), isSRGBGColorSpace ? STBIR_COLORSPACE_SRGB : STBIR_COLORSPACE_LINEAR, nullptr);
        return ImageConversion::convertImageDataToFormat(resizedImageData, ImageFormat::RGBA32323232F,format,resizeWidth, resizeHeight);
    }
}

std::unique_ptr<VTFBuilder> VTFBuilder::open(const std::string &path) {

    int32_t x, y, n;
    auto buff = stbi_load(path.data(),&x,&y,&n,4);

    if(!buff)
        return nullptr;

    auto uniPTR = open(buff, ImageFormatDetails::getDataLength(ImageFormat::RGBA8888,x,y));
    uniPTR->setDimensions(x,y);
    uniPTR->setInputDataFormat(ImageFormat::RGBA8888);
    uniPTR->setVTFFormat(ImageFormat::RGBA8888);
    stbi_image_free(buff);

    return std::move(uniPTR);
}

