#include <vtfpp/PVR.h>

#include <vector>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vtfpp;

PVR::PVR(std::span<const std::byte> pvrData) {
	BufferStreamReadOnly stream{pvrData};

	// skip GBIX ("global index") chunk found in many examples
	if (stream.read<uint32_t>() == GBIX_SIGNATURE) {
		uint32_t len = stream.read<uint32_t>();
		stream.skip(len);
	}

	if (stream.read<uint32_t>() != PVR_SIGNATURE) {
		return;
	}

	auto len = stream.read<uint32_t>();
	auto type = stream.read<uint32_t>();

	this->imageType = static_cast<PVR::ImageType>(type & 0xFF00 >> 8);
	this->pixelType = static_cast<PVR::PixelType>(type & 0xFF);

	stream >> this->width;
	stream >> this->height;

	if (this->hasMips()) {
		this->mipCount = 4;
	}

	switch (this->pixelType) {
		case PIXEL_TYPE_ARGB1555:
			break;
		case PIXEL_TYPE_RGB565:
			this->format = ImageFormat::RGB565;
			break;
		case PIXEL_TYPE_ARGB4444:
			break;
		case PIXEL_TYPE_YUV422:
			break;
		case PIXEL_TYPE_BUMP:
			break;
		case PIXEL_TYPE_RGB555:
			break;
		case PIXEL_TYPE_YUV420:
			break;
	}

	switch (this->imageType) {
		case IMAGE_TYPE_NONE:
			break;
		case IMAGE_TYPE_TWIDDLED:
			break;
		case IMAGE_TYPE_TWIDDLED_MM:
			break;
		case IMAGE_TYPE_VQ:
			break;
		case IMAGE_TYPE_VQ_MM:
			break;
		case IMAGE_TYPE_PAL4:
			break;
		case IMAGE_TYPE_PAL4_MM:
			break;
		case IMAGE_TYPE_PAL8:
			break;
		case IMAGE_TYPE_PAL8_MM:
			break;
		case IMAGE_TYPE_RECTANGULAR:
			break;
		case IMAGE_TYPE_RECTANGULAR_MM:
			break;
		case IMAGE_TYPE_STRIDE:
			break;
		case IMAGE_TYPE_STRIDE_MM:
			break;
		case IMAGE_TYPE_RECTANGULAR_TWIDDLED:
			break;
		case IMAGE_TYPE_BMP:
			break;
		case IMAGE_TYPE_BMP_MM:
			break;
		case IMAGE_TYPE_SMALLVQ:
			break;
		case IMAGE_TYPE_SMALLVQ_MM:
			break;
	}
}

PVR::PVR(const std::filesystem::path& pvrPath)
		: PVR(fs::readFileBuffer(pvrPath)) {}

PVR::operator bool() const {
	return this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0;
}

uint16_t PVR::getWidth() const {
	return this->width;
}

uint16_t PVR::getHeight() const {
	return this->height;
}

bool PVR::hasImageData() const {
	return this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0;
}

bool PVR::hasMips() const {
	return this->imageType == IMAGE_TYPE_TWIDDLED_MM ||
			this->imageType == IMAGE_TYPE_VQ_MM ||
			this->imageType == IMAGE_TYPE_PAL4_MM ||
			this->imageType == IMAGE_TYPE_PAL8_MM ||
			this->imageType == IMAGE_TYPE_RECTANGULAR_MM ||
			this->imageType == IMAGE_TYPE_STRIDE_MM ||
			this->imageType == IMAGE_TYPE_BMP_MM ||
			this->imageType == IMAGE_TYPE_SMALLVQ_MM;
}

bool PVR::isTwiddled() const {
	return this->imageType == IMAGE_TYPE_TWIDDLED ||
			this->imageType == IMAGE_TYPE_TWIDDLED_MM ||
			this->imageType == IMAGE_TYPE_RECTANGULAR_TWIDDLED;
}

bool PVR::isPaletted() const {
	return this->imageType == IMAGE_TYPE_PAL4 ||
			this->imageType == IMAGE_TYPE_PAL4_MM ||
			this->imageType == IMAGE_TYPE_PAL8 ||
			this->imageType == IMAGE_TYPE_PAL8_MM;
}

ImageFormat PVR::getFormat() const {
	return this->format;
}

uint8_t PVR::getMipCount() const {
	return this->mipCount;
}

std::span<const std::byte> PVR::getImageDataRaw(uint8_t mip) const {
	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, 0, 1, 0, 1, this->width, this->height, 0, 1)) {
		return this->imageData.subspan(offset, length);
	}
	return {};
}

std::span<std::byte> PVR::getImageDataRaw(uint8_t mip) {
	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, 0, 1, 0, 1, this->width, this->height, 0, 1)) {
		return this->imageData.subspan(offset, length);
	}
	return {};
}

std::vector<std::byte> PVR::getImageDataAs(ImageFormat newFormat, uint8_t mip) const {
	const auto rawImageData = this->getImageDataRaw(mip);
	if (rawImageData.empty()) {
		return {};
	}
#if 0
	if (this->format == ImageFormat::P8) {
		if (const auto paletteData = this->getPaletteResourceFrame(frame); !paletteData.empty()) {
			const auto [mipWidth, mipHeight] = ImageDimensions::getMipDims(mip, this->width, this->height);
			return ImageConversion::convertImageDataToFormat(ImageQuantize::convertP8ImageDataToBGRA8888(paletteData, rawImageData), ImageFormat::BGRA8888, newFormat, mipWidth, mipHeight);
		}
	}
#endif
	const auto [mipWidth, mipHeight] = ImageDimensions::getMipDims(mip, this->width, this->height);
	return ImageConversion::convertImageDataToFormat(rawImageData, this->format, newFormat, mipWidth, mipHeight);
}

std::vector<std::byte> PVR::getImageDataAsRGBA8888(uint8_t mip) const {
	return this->getImageDataAs(ImageFormat::RGBA8888, mip);
}
