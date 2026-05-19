// ReSharper disable CppRedundantQualifier

#include <vtfpp/PSFrames.h>

#include <BufferStream.h>
#include <sourcepp/FS.h>
#include <vtfpp/ImageConversion.h>
#include <vtfpp/ImagePixel.h>

using namespace sourcepp;
using namespace vtfpp;

PSFrames::PSFrames(std::vector<std::byte>&& psFramesData) {
	this->data = std::move(psFramesData);
	BufferStreamReadOnly stream{this->data};
	stream
		.read(this->frameCount)
		.read(this->fps);

	this->frames.reserve(this->frameCount);
	for (uint32_t i = 0; i < this->frameCount; i++) {
		auto& [width, height, paletteData, imageData] = this->frames.emplace_back();

		if (stream.read<uint32_t>() != 65793) {
			return;
		}
		stream.skip(1);
		if (const auto palettePixelCount = stream.read<uint16_t>(); palettePixelCount != 256) {
			return;
		}
		if (const auto palettePixelBits = stream.read<uint16_t>(); palettePixelBits != 24) {
			return;
		}
		stream.skip(3) >> width >> height;
		if (const auto paletteIndexBits = stream.read<uint8_t>(); paletteIndexBits != 8) {
			return;
		}
		if (const auto decodedPixelBits = stream.read<uint16_t>(); decodedPixelBits != 32) {
			return;
		}

		paletteData = stream.read_span<std::byte>(256 * sizeof(ImagePixel::BGR888)); // static_cast<uint32_t>(palettePixelCount) * palettePixelBits / 8
		imageData = stream.read_span<std::byte>(width * height); // static_cast<uint32_t>(width) * height * paletteIndexBits / 8
	}

	this->opened = true;
}

PSFrames::PSFrames(std::span<const std::byte> psFramesData)
		: PSFrames(std::vector<std::byte>{psFramesData.begin(), psFramesData.end()}) {}

PSFrames::PSFrames(const std::filesystem::path& psFramesPath)
		: PSFrames(fs::readFileBuffer(psFramesPath)) {}

PSFrames::operator bool() const {
	return this->opened;
}

uint32_t PSFrames::getFrameCount() const {
	return this->frameCount;
}

uint32_t PSFrames::getFPS() const {
	return this->fps;
}

uint16_t PSFrames::getWidth(uint32_t frame) const {
	return this->frames.at(frame).width;
}

uint16_t PSFrames::getHeight(uint32_t frame) const {
	return this->frames.at(frame).height;
}

std::span<const std::byte> PSFrames::getPaletteDataRaw(uint32_t frame) const {
	return this->frames.at(frame).paletteData;
}

std::span<std::byte> PSFrames::getPaletteDataRaw(uint32_t frame) {
	return this->frames.at(frame).paletteData;
}

std::vector<std::byte> PSFrames::getPaletteDataAs(ImageFormat newFormat, uint32_t frame) const {
	return ImageConversion::convertImageDataToFormat(this->frames.at(frame).paletteData, ImageFormat::BGR888, newFormat, 16, 16);
}

std::span<const std::byte> PSFrames::getImageDataRaw(uint32_t frame) const {
	return this->frames.at(frame).imageData;
}

std::span<std::byte> PSFrames::getImageDataRaw(uint32_t frame) {
	return this->frames.at(frame).imageData;
}

std::vector<std::byte> PSFrames::getImageDataAs(ImageFormat newFormat, uint32_t frame) const {
	const auto& frameData = this->frames.at(frame);
	return ImageConversion::convertImageDataToFormat(this->getImageDataAsBGR888(frame), ImageFormat::BGR888, newFormat, frameData.width, frameData.height);
}

std::vector<std::byte> PSFrames::getImageDataAsBGR888(uint32_t frame) const {
	const auto& [width, height, paletteData, imageData] = this->frames.at(frame);
	const std::span palettePixelData{reinterpret_cast<const ImagePixel::BGR888*>(paletteData.data()), 256};

	std::vector<std::byte> out;
	out.resize(width * height * sizeof(ImagePixel::BGR888));
	BufferStream stream{out};
	for (uint32_t i = 0; i < width * height; i++) {
		stream << palettePixelData[static_cast<uint8_t>(imageData[i])];
	}
	return out;
}
