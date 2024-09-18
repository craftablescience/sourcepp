#include <vtfpp/PPL.h>

#include <ranges>

#include <BufferStream.h>

using namespace sourcepp;
using namespace vtfpp;

PPL::PPL(uint32_t checksum_, ImageFormat format_, uint32_t version_)
		: version(version_)
		, checksum(checksum_)
		, format(format_) {}

PPL::PPL(std::span<const std::byte> pplData) {
	BufferStreamReadOnly reader{pplData.data(), pplData.size()};
	reader >> this->version >> this->checksum >> this->format;

	auto imageCount = reader.read<uint32_t>();
	reader.skip<uint32_t>(4);
	for (uint32_t i = 0; i < imageCount; i++) {
		auto lod = reader.read<uint32_t>();
		auto offset = reader.read<uint32_t>();
		auto length = reader.read<uint32_t>();
		auto width = reader.read<uint32_t>();
		auto height = reader.read<uint32_t>();
		reader.skip<uint32_t>(3);

		this->images[lod] = {
			.width = width,
			.height = height,
			.data = {pplData.data() + offset, pplData.data() + offset + length},
		};
	}
}

PPL::PPL(const std::string& pplPath)
		: PPL(fs::readFileBuffer(pplPath)) {}

PPL::operator bool() const {
	return !this->images.empty();
}

uint32_t PPL::getVersion() const {
	return this->version;
}

void PPL::setVersion(uint32_t newVersion) {
	this->version = newVersion;
}

uint32_t PPL::getChecksum() const {
	return this->checksum;
}

void PPL::setChecksum(uint32_t newChecksum) {
	this->checksum = newChecksum;
}

ImageFormat PPL::getFormat() const {
	return this->format;
}

void PPL::setFormat(ImageFormat newFormat) {
	for (auto& [lod, image] : this->images) {
		image.data = ImageConversion::convertImageDataToFormat(image.data, this->format, newFormat, image.width, image.height);
	}
	this->format = newFormat;
}

bool PPL::hasImageForLOD(uint32_t lod) const {
	return this->images.contains(lod);
}

std::vector<uint32_t> PPL::getImageLODs() const {
	auto view = std::views::keys(this->images);
	return {view.begin(), view.end()};
}

const PPL::Image* PPL::getImageRaw(uint32_t lod) const {
	if (!this->hasImageForLOD(lod)) {
		return nullptr;
	}
	return &this->images.at(lod);
}

std::optional<PPL::Image> PPL::getImageAs(ImageFormat newFormat, uint32_t lod) const {
	if (!this->hasImageForLOD(lod)) {
		return std::nullopt;
	}
	const auto& image = this->images.at(lod);
	return Image{
		.width = image.width,
		.height = image.height,
		.data = ImageConversion::convertImageDataToFormat(image.data, this->format, newFormat, image.width, image.height),
	};
}

std::optional<PPL::Image> PPL::getImageAsRGB888(uint32_t lod) const {
	return this->getImageAs(ImageFormat::RGB888, lod);
}

bool PPL::setImage(std::span<const std::byte> imageData, ImageFormat format_, uint32_t width, uint32_t height, uint32_t lod) {
	if (!width || !height) {
		return false;
	}
	this->images[lod] = {
		.width = width,
		.height = height,
		.data = format_ == this->format ? std::vector<std::byte>{imageData.begin(), imageData.end()} : ImageConversion::convertImageDataToFormat(imageData, format_, this->format, width, height),
	};
	return true;
}

bool PPL::setImage(std::span<const std::byte> imageData, ImageFormat format_, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, ImageConversion::ResizeFilter filter) {
	if (!width || !height || !resizedWidth || !resizedHeight) {
		return false;
	}
	const auto unresizedData = format_ == this->format ? std::vector<std::byte>{imageData.begin(), imageData.end()} : ImageConversion::convertImageDataToFormat(imageData, format_, this->format, width, height);
	this->images[lod] = {
		.width = width,
		.height = height,
		.data = ImageConversion::resizeImageData(unresizedData, this->format, width, resizedWidth, height, resizedHeight, false, filter),
	};
	return true;
}

bool PPL::setImage(const std::string& imagePath, uint32_t lod) {
	ImageFormat inputFormat;
	int inputWidth, inputHeight, inputFrameCount;
	auto imageData_ = ImageConversion::convertFileToImageData(fs::readFileBuffer(imagePath), inputFormat, inputWidth, inputHeight, inputFrameCount);

	// Unable to decode file
	if (inputFormat == ImageFormat::EMPTY || !inputWidth || !inputHeight || !inputFrameCount) {
		return false;
	}

	// One frame (normal)
	if (inputFrameCount == 1) {
		return this->setImage(imageData_, inputFormat, inputWidth, inputHeight, lod);
	}

	// Multiple frames (GIF) - discard extra frames
	return this->setImage({imageData_.data(), ImageFormatDetails::getDataLength(inputFormat, inputWidth, inputHeight)}, inputFormat, inputWidth, inputHeight, lod);
}

bool PPL::setImage(const std::string& imagePath, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, ImageConversion::ResizeFilter filter) {
	ImageFormat inputFormat;
	int inputWidth, inputHeight, inputFrameCount;
	auto imageData_ = ImageConversion::convertFileToImageData(fs::readFileBuffer(imagePath), inputFormat, inputWidth, inputHeight, inputFrameCount);

	// Unable to decode file
	if (inputFormat == ImageFormat::EMPTY || !inputWidth || !inputHeight || !inputFrameCount) {
		return false;
	}

	// One frame (normal)
	if (inputFrameCount == 1) {
		return this->setImage(imageData_, inputFormat, inputWidth, inputHeight, resizedWidth, resizedHeight, lod, filter);
	}

	// Multiple frames (GIF) - discard extra frames
	return this->setImage({imageData_.data(), ImageFormatDetails::getDataLength(inputFormat, inputWidth, inputHeight)}, inputFormat, inputWidth, inputHeight, resizedWidth, resizedHeight, lod, filter);
}

std::vector<std::byte> PPL::saveImageToFile(uint32_t lod, ImageConversion::FileFormat fileFormat) const {
	if (auto image = this->getImageRaw(lod)) {
		return ImageConversion::convertImageDataToFile(image->data, this->format, image->width, image->height, fileFormat);
	}
	return {};
}

bool PPL::saveImageToFile(const std::string& imagePath, uint32_t lod, ImageConversion::FileFormat fileFormat) const {
	if (auto data = this->saveImageToFile(lod, fileFormat); !data.empty()) {
		return fs::writeFileBuffer(imagePath, data);
	}
	return false;
}

std::vector<std::byte> PPL::bake() {
	static constexpr auto ALIGNMENT = 512;

	std::vector<std::byte> out;
	BufferStream writer{out};

	static constexpr auto HEADER_SIZE = sizeof(uint32_t) * 8;
	writer << this->version << this->checksum << this->format;
	writer.write<uint32_t>(this->images.size());

	while (writer.tell() < ALIGNMENT) {
		writer.write<uint32_t>(0);
	}
	writer.seek(HEADER_SIZE);

	uint32_t currentOffset = ALIGNMENT;
	for (const auto& [lod, image] : this->images) {
		writer << lod << currentOffset << static_cast<uint32_t>(image.data.size()) << image.width << image.height;
		for (int i = 0; i < 3; i++) {
			writer.write<uint32_t>(0);
		}
		auto seekPoint = writer.tell();
		writer.seek_u(currentOffset).write(image.data);
		const auto alignment = math::getPaddingForAlignment(ALIGNMENT, writer.tell());
		for (int i = 0; i < alignment; i++) {
			writer.write<uint8_t>(0);
		}
		writer.seek_u(seekPoint);
		currentOffset += image.data.size() + alignment;
	}
	writer.seek(0, std::ios::end);

	out.resize(writer.size());
	return out;
}

bool PPL::bake(const std::string& pplPath) {
	return fs::writeFileBuffer(pplPath, this->bake());
}
