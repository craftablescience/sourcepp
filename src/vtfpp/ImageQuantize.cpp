#include <vtfpp/ImageQuantize.h>

using namespace vtfpp;

std::vector<std::byte> ImageQuantize::convertP8ImageDataToBGRA8888(std::span<const std::byte> paletteData, std::span<const std::byte> imageData) {
	if (paletteData.size() != 256 * sizeof(ImagePixel::BGRA8888)) {
		return {};
	}

	std::span palettePixelData{reinterpret_cast<const ImagePixel::BGRA8888*>(paletteData.data()), 256};

	std::vector<std::byte> out;
	out.resize(imageData.size() * sizeof(ImagePixel::BGRA8888));
	BufferStream stream{out};
	for (const auto index : imageData) {
		stream << palettePixelData[static_cast<uint8_t>(index)];
	}
	return out;
}
