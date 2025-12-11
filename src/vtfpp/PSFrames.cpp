// ReSharper disable CppRedundantQualifier

#include <vtfpp/PSFrames.h>

#include <BufferStream.h>
#include <sourcepp/FS.h>
#include <vtfpp/ImageConversion.h>

using namespace sourcepp;
using namespace vtfpp;

PSFrames::PSFrames(std::vector<std::byte>&& psFramesData) {
	this->data = std::move(psFramesData);
	BufferStreamReadOnly stream{this->data};
	stream
		.read(this->frameCount)
		.read(this->fps)
		.skip<uint32_t>(3) // unknown
		.read(this->width)
		.read(this->height)
		.skip<uint8_t>() // probably channel bit width
		.skip<uint16_t>(); // probably uncompressed pixel width
}

PSFrames::PSFrames(std::span<const std::byte> psFramesData)
		: PSFrames(std::vector<std::byte>{psFramesData.begin(), psFramesData.end()}) {}

PSFrames::PSFrames(const std::filesystem::path& psFramesPath)
		: PSFrames(fs::readFileBuffer(psFramesPath)) {}

uint32_t PSFrames::getFrameCount() const {
	return this->frameCount;
}

uint32_t PSFrames::getFPS() const {
	return this->fps;
}

uint16_t PSFrames::getWidth() const {
	return this->width;
}

uint16_t PSFrames::getHeight() const {
	return this->height;
}

std::vector<std::byte> PSFrames::getImageDataAs(ImageFormat newFormat, uint32_t frame) const {
	return ImageConversion::convertImageDataToFormat(this->getImageDataAsBGR888(frame), ImageFormat::BGR888, newFormat, this->width, this->height);
}

std::vector<std::byte> PSFrames::getImageDataAsBGR888(uint32_t frame) const {
	BufferStreamReadOnly stream{this->data.data(), this->data.size()};
	const auto palette = stream.seek(PSFrames::getFrameOffset(frame)).read_bytes(256 * sizeof(ImagePixel::BGR888));
	const std::span palettePixelData{reinterpret_cast<const ImagePixel::BGR888*>(palette.data()), 256};

	std::vector<std::byte> out;
	out.resize(this->width * this->height * sizeof(ImagePixel::BGR888));
	BufferStream outStream{out};
	for (uint32_t i = 0; i < this->width * this->height; i++) {
		outStream << palettePixelData[stream.read<uint8_t>()];
	}
	return out;
}

uint32_t PSFrames::getFrameOffset(uint32_t frame) const {
	static constexpr auto headerSize = sizeof(uint32_t) * 5 + sizeof(uint16_t) * 3 + sizeof(uint8_t);
	static constexpr auto sampleSize = 19; // I'm assuming this is an audio sample, it's not for alignment
	return headerSize + (this->getFramePaletteAndImageSize() + sampleSize) * frame;
}

uint32_t PSFrames::getFramePaletteAndImageSize() const {
	return 256 * sizeof(ImagePixel::BGR888) + this->width * this->height;
}
