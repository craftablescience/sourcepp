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

ImageConversion::ResizeMethod DDS::getImageWidthResizeMethod() const {
	return this->imageWidthResizeMethod;
}

ImageConversion::ResizeMethod DDS::getImageHeightResizeMethod() const {
	return this->imageHeightResizeMethod;
}

void DDS::setImageResizeMethods(ImageConversion::ResizeMethod imageWidthResizeMethod_, ImageConversion::ResizeMethod imageHeightResizeMethod_) {
	this->imageWidthResizeMethod = imageWidthResizeMethod_;
	this->imageHeightResizeMethod = imageHeightResizeMethod_;
}

void DDS::setImageWidthResizeMethod(ImageConversion::ResizeMethod imageWidthResizeMethod_) {
	this->imageWidthResizeMethod = imageWidthResizeMethod_;
}

void DDS::setImageHeightResizeMethod(ImageConversion::ResizeMethod imageHeightResizeMethod_) {
	this->imageHeightResizeMethod = imageHeightResizeMethod_;
}

uint16_t DDS::getWidth(uint8_t mip) const {
	return ImageDimensions::getMipDim(mip, this->width);
}

uint16_t DDS::getPaddedWidth(uint8_t mip) const {
	return ImageDimensions::getMipDim(mip, this->width, ImageFormatDetails::compressed(this->format));
}

uint16_t DDS::getHeight(uint8_t mip) const {
	return ImageDimensions::getMipDim(mip, this->height);
}

uint16_t DDS::getPaddedHeight(uint8_t mip) const {
	return ImageDimensions::getMipDim(mip, this->height, ImageFormatDetails::compressed(this->format));
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

bool DDS::setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality) {
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
		if (const auto newMipCount = ImageDimensions::getMaximumMipCount(resizedWidth, resizedHeight, this->depth); newMipCount <= mip) {
			mip = newMipCount - 1;
		}
		if (face > 6) {
			return false;
		}
		this->regenerateImageData(format_, resizedWidth, resizedHeight, mip + 1, frame + 1, face ? (face < 6 ? 6 : face) : 0, slice + 1);
	}

	const auto faceCount = this->getFaceCount();
	if (this->mipCount <= mip || this->frameCount <= frame || faceCount <= face || this->depth <= slice) {
		return false;
	}

	const auto* imageResource = this->getResource(Resource::TYPE_IMAGE_DATA);
	if (!imageResource) {
		return false;
	}
	if (uint32_t offset, length; ImageFormatDetails::getDataPosition(offset, length, this->format, mip, this->mipCount, frame, this->frameCount, face, faceCount, this->width, this->height, slice, this->depth)) {
		std::vector<std::byte> image{imageData_.begin(), imageData_.end()};
		const auto [newWidth, newHeight] = ImageDimensions::getMipDims(mip, this->width, this->height);
		if (width_ != newWidth || height_ != newHeight) {
			image = ImageConversion::resizeImageData(image, format_, width_, newWidth, height_, newHeight, this->isSRGB(), this->getFlagsExtra() & FLAG_EXTRA_USING_PREMULTIPLIED_ALPHA_RESIZE, filter);
		}
		if (format_ != this->format) {
			image = ImageConversion::convertImageDataToFormat(image, format_, this->format, newWidth, newHeight, quality);
		}
		std::memcpy(imageResource->data.data() + offset, image.data(), image.size());
	}
	return true;
}

bool DDS::setImage(const std::filesystem::path& imagePath, ImageConversion::ResizeFilter filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality, ImageConversion::ResizeBounds resizeBounds) {
	ImageFormat inputFormat;
	int inputWidth, inputHeight, inputFrameCount;
	auto imageData_ = ImageConversion::convertFileToImageData(fs::readFileBuffer(imagePath), inputFormat, inputWidth, inputHeight, inputFrameCount);

	// Unable to decode file
	if (imageData_.empty() || inputFormat == ImageFormat::EMPTY || !inputWidth || !inputHeight || !inputFrameCount) {
		return false;
	}

	// Image dimension overrides
	auto [requestedResizeWidth, requestedResizeHeight] = resizeBounds.clamp(inputWidth, inputHeight);

	// One frame (normal)
	if (inputFrameCount == 1) {
		if (requestedResizeWidth != inputWidth || requestedResizeHeight != inputHeight) {
			const auto imageDataResized = ImageConversion::resizeImageData(imageData_, inputFormat, inputWidth, requestedResizeWidth, inputHeight, requestedResizeHeight, !ImageFormatDetails::large(inputFormat), this->getFlagsExtra() & FLAG_EXTRA_USING_PREMULTIPLIED_ALPHA_RESIZE, resizeBounds.resizeFilter, resizeBounds.resizeEdge);
			return this->setImage(imageDataResized, inputFormat, requestedResizeWidth, requestedResizeHeight, filter, mip, frame, face, slice, quality);
		}
		return this->setImage(imageData_, inputFormat, inputWidth, inputHeight, filter, mip, frame, face, slice, quality);
	}

	// Multiple frames (GIF)
	bool allSuccess = true;
	const auto frameSize = ImageFormatDetails::getDataLength(inputFormat, inputWidth, inputHeight);
	for (int currentFrame = 0; currentFrame < inputFrameCount; currentFrame++) {
		std::span currentFrameData{imageData_.data() + currentFrame * frameSize, imageData_.data() + currentFrame * frameSize + frameSize};
		if (requestedResizeWidth != inputWidth || requestedResizeHeight != inputHeight) {
			const auto currentFrameResized = ImageConversion::resizeImageData(currentFrameData, inputFormat, inputWidth, requestedResizeWidth, inputHeight, requestedResizeHeight, !ImageFormatDetails::large(inputFormat), this->getFlagsExtra() & FLAG_EXTRA_USING_PREMULTIPLIED_ALPHA_RESIZE, resizeBounds.resizeFilter, resizeBounds.resizeEdge);
			if (!this->setImage(currentFrameResized, inputFormat, requestedResizeWidth, requestedResizeHeight, filter, mip, frame + currentFrame, face, slice, quality)) {
				allSuccess = false;
			}
		} else if (!this->setImage(currentFrameData, inputFormat, inputWidth, inputHeight, filter, mip, frame + currentFrame, face, slice, quality)) {
			allSuccess = false;
		}
		if (currentFrame == 0 && this->frameCount < frame + inputFrameCount) {
			// Call this after setting the first image, this function is a no-op if no image data is present yet
			this->setFrameCount(frame + inputFrameCount);
		}
	}
	return allSuccess;
}

bool DDS::create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::filesystem::path& ddsPath, const CreationOptions& options) {
	DDS writer;
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	if (const auto [requestedResizeWidth, requestedResizeHeight] = options.resizeBounds.clamp(width, height); requestedResizeWidth != width || requestedResizeHeight != height) {
		const auto imageDataResized = ImageConversion::resizeImageData(imageData, format, width, requestedResizeWidth, height, requestedResizeHeight, !ImageFormatDetails::large(format), writer.getFlagsExtra() & FLAG_EXTRA_USING_PREMULTIPLIED_ALPHA_RESIZE, options.resizeBounds.resizeFilter, options.resizeBounds.resizeEdge);
		if (!writer.setImage(imageDataResized, format, requestedResizeWidth, requestedResizeHeight, options.filter)) {
			return false;
		}
	} else if (!writer.setImage(imageData, format, width, height, options.filter)) {
		return false;
	}
	if (!createInternal(writer, options)) {
		return false;
	}
	return writer.bake(ddsPath);
}

bool DDS::create(ImageFormat format, uint16_t width, uint16_t height, const std::filesystem::path& ddsPath, const CreationOptions& options) {
	std::vector<std::byte> imageData;
	const auto [requestedResizeWidth, requestedResizeHeight] = options.resizeBounds.clamp(width, height);
	imageData.resize(static_cast<uint32_t>(requestedResizeWidth) * requestedResizeHeight * ImageFormatDetails::bpp(format) / 8);
	return create(imageData, format, requestedResizeWidth, requestedResizeHeight, ddsPath, options);
}

DDS DDS::create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options) {
	DDS writer;
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	if (const auto [requestedResizeWidth, requestedResizeHeight] = options.resizeBounds.clamp(width, height); requestedResizeWidth != width || requestedResizeHeight != height) {
		const auto imageDataResized = ImageConversion::resizeImageData(imageData, format, width, requestedResizeWidth, height, requestedResizeHeight, !ImageFormatDetails::large(format), writer.getFlagsExtra() & FLAG_EXTRA_USING_PREMULTIPLIED_ALPHA_RESIZE, options.resizeBounds.resizeFilter, options.resizeBounds.resizeEdge);
		if (!writer.setImage(imageDataResized, format, requestedResizeWidth, requestedResizeHeight, options.filter)) {
			writer.opened = false;
			return writer;
		}
	} else if (!writer.setImage(imageData, format, width, height, options.filter)) {
		writer.opened = false;
		return writer;
	}
	if (!createInternal(writer, options)) {
		writer.opened = false;
	}
	return writer;
}

DDS DDS::create(ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options) {
	std::vector<std::byte> imageData;
	const auto [requestedResizeWidth, requestedResizeHeight] = options.resizeBounds.clamp(width, height);
	imageData.resize(static_cast<uint32_t>(requestedResizeWidth) * requestedResizeHeight * ImageFormatDetails::bpp(format) / 8);
	return create(imageData, format, requestedResizeWidth, requestedResizeHeight, options);
}

bool DDS::create(const std::filesystem::path& imagePath, const std::filesystem::path& ddsPath, const CreationOptions& options) {
	DDS writer;
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	if (!writer.setImage(imagePath, options.filter, 0, 0, 0, 0, options.compressedFormatQuality, options.resizeBounds)) {
		return false;
	}
	if (!createInternal(writer, options)) {
		return false;
	}
	return writer.bake(ddsPath);
}

DDS DDS::create(const std::filesystem::path& imagePath, const CreationOptions& options) {
	DDS writer;
	writer.setImageResizeMethods(options.widthResizeMethod, options.heightResizeMethod);
	if (!writer.setImage(imagePath, options.filter, 0, 0, 0, 0, options.compressedFormatQuality, options.resizeBounds)) {
		writer.opened = false;
		return writer;
	}
	if (!createInternal(writer, options)) {
		writer.opened = false;
	}
	return writer;
}
