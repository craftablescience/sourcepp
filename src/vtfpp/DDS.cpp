#include <vtfpp/DDS.h>

#include <vector>

#include <BufferStream.h>
#include <sourcepp/FS.h>

using namespace sourcepp;
using namespace vtfpp;

constexpr uint32_t DXT1_SIGNATURE = sourcepp::parser::binary::makeFourCC("DXT1");
constexpr uint32_t DXT2_SIGNATURE = sourcepp::parser::binary::makeFourCC("DXT2");
constexpr uint32_t DXT3_SIGNATURE = sourcepp::parser::binary::makeFourCC("DXT3");
constexpr uint32_t DXT4_SIGNATURE = sourcepp::parser::binary::makeFourCC("DXT4");
constexpr uint32_t DXT5_SIGNATURE = sourcepp::parser::binary::makeFourCC("DXT5");
constexpr uint32_t DX10_SIGNATURE = sourcepp::parser::binary::makeFourCC("DX10");

DDS::DDS(std::span<const std::byte> ddsData) {
	BufferStreamReadOnly stream{ddsData};

	if (stream.read<uint32_t>() != DDS_SIGNATURE) {
		return;
	}

	const auto DDS_PIXELFORMAT_LENGTH = sizeof(uint32_t) * 8;
	const auto DDS_HEADER_LENGTH = sizeof(uint32_t) * 23 + sizeof(DDS_PIXELFORMAT_LENGTH);

	// Check header size
	uint32_t headerLength = stream.read<uint32_t>();
	if (headerLength != DDS_HEADER_LENGTH) {
		return;
	}

	// Read header fields
	uint32_t headerFlags = stream.read<uint32_t>();
	uint32_t imageHeight = stream.read<uint32_t>();
	uint32_t imageWidth = stream.read<uint32_t>();
	uint32_t imagePitch = stream.read<uint32_t>();
	uint32_t imageDepth = stream.read<uint32_t>();
	uint32_t imageMips = stream.read<uint32_t>();

	// Skip reserved section
	stream.skip_u(sizeof(uint32_t) * 11);

	// Check pixel format subheader size
	uint32_t pixelHeaderLength = stream.read<uint32_t>();
	if (pixelHeaderLength != DDS_PIXELFORMAT_LENGTH) {
		return;
	}

	// Read pixel format subheader fields
	uint32_t pixelHeaderFlags = stream.read<uint32_t>();
	uint32_t pixelHeaderFourCC = stream.read<uint32_t>();
	uint32_t pixelHeaderBitCount = stream.read<uint32_t>();
	uint32_t pixelHeaderRedBitMask = stream.read<uint32_t>();
	uint32_t pixelHeaderGreenBitMask = stream.read<uint32_t>();
	uint32_t pixelHeaderBlueBitMask = stream.read<uint32_t>();
	uint32_t pixelHeaderAlphaBitMask = stream.read<uint32_t>();

	// Read the rest of the main header
	uint32_t headerCaps0 = stream.read<uint32_t>();
	uint32_t headerCaps1 = stream.read<uint32_t>();
	uint32_t headerCaps2 = stream.read<uint32_t>();
	uint32_t headerCaps3 = stream.read<uint32_t>();

	// Skip reserved value
	stream.skip_u(sizeof(uint32_t));

	ImageFormat imageFormat = ImageFormat::EMPTY;

	// Check if we need to parse DX10 header
	if (pixelHeaderFourCC == DX10_SIGNATURE) {
		const auto DX10_HEADER_LENGTH = sizeof(uint32_t) * 5;

		uint32_t dx10HeaderFormat = stream.read<uint32_t>();
		uint32_t dx10HeaderResourceDimension = stream.read<uint32_t>();
		uint32_t dx10HeaderMiscFlags0 = stream.read<uint32_t>();
		uint32_t dx10HeaderArraySize = stream.read<uint32_t>();
		uint32_t dx10HeaderMiscFlags1 = stream.read<uint32_t>();

	} else {
		// Get image format
		switch (pixelHeaderFourCC) {
			case DXT1_SIGNATURE:
				imageFormat = ImageFormat::DXT1;
				break;
			case DXT2_SIGNATURE:
				imageFormat = ImageFormat::DXT3;
				break;
			case DXT3_SIGNATURE:
				imageFormat = ImageFormat::DXT3;
				break;
			case DXT4_SIGNATURE:
				imageFormat = ImageFormat::DXT5;
				break;
			case DXT5_SIGNATURE:
				imageFormat = ImageFormat::DXT5;
				break;
		}
	}

	// Couldn't determine image format
	if (imageFormat == ImageFormat::EMPTY) {
		return;
	}

	// Setup final fields
	this->width = imageWidth;
	this->height = imageHeight;
	this->format = imageFormat;
	this->mipCount = imageMips;

	// Read image data
	this->data = stream.read_bytes(stream.size() - stream.tell());
	this->imageData = {this->data.begin(), this->data.end()};
}

DDS::DDS() {
	this->opened = true;
}

DDS::DDS(const DDS& other) {
	*this = other;
}

DDS& DDS::operator=(const DDS& other) {
	this->opened = other.opened;
	this->data = other.data;
	this->imageData = {this->data.begin(), this->data.end()};
	this->width = other.width;
	this->height = other.height;
	this->format = other.format;
	this->mipCount = other.mipCount;

	return *this;
}

DDS::DDS(const std::filesystem::path& ddsPath)
		: DDS(fs::readFileBuffer(ddsPath)) {}

DDS::operator bool() const {
	return this->opened;
}

uint16_t DDS::getWidth() const {
	return this->width;
}

uint16_t DDS::getHeight() const {
	return this->height;
}

bool DDS::hasImageData() const {
	return this->format != ImageFormat::EMPTY && this->width > 0 && this->height > 0;
}

ImageFormat DDS::getFormat() const {
	return this->format;
}

uint8_t DDS::getMipCount() const {
	return this->mipCount;
}

std::span<const std::byte> DDS::getImageDataRaw(uint8_t mip) const {
	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, 0, 1, 0, 1, this->width, this->height, 0, 1)) {
		return this->imageData.subspan(offset, length);
	}
	return {};
}

std::span<std::byte> DDS::getImageDataRaw(uint8_t mip) {
	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, 0, 1, 0, 1, this->width, this->height, 0, 1)) {
		return this->imageData.subspan(offset, length);
	}
	return {};
}

std::vector<std::byte> DDS::getImageDataAs(ImageFormat newFormat, uint8_t mip) const {
	const auto rawImageData = this->getImageDataRaw(mip);
	if (rawImageData.empty()) {
		return {};
	}
	const auto [mipWidth, mipHeight] = ImageDimensions::getMipDims(mip, this->width, this->height);
	return ImageConversion::convertImageDataToFormat(rawImageData, this->format, newFormat, mipWidth, mipHeight);
}

std::vector<std::byte> DDS::getImageDataAsRGBA8888(uint8_t mip) const {
	return this->getImageDataAs(ImageFormat::RGBA8888, mip);
}
