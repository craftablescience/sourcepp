#pragma once

#include <filesystem>
#include <span>
#include <vector>

#include <vtfpp/ImageFormats.h>

namespace vtfpp {

/// Parses "frames" files as seen in The Orange Box PS3 port, in the TF/MOVIES.GRP file group.
class PSFrames {
protected:
	struct Frame {
		//uint16_t palettePixelCount;
		//uint16_t palettePixelBits;
		uint16_t width;
		uint16_t height;
		//uint8_t paletteIndexBits;
		//uint16_t decodedPixelBits;

		std::span<std::byte> paletteData;
		std::span<std::byte> imageData;
	};

public:
	explicit PSFrames(std::vector<std::byte>&& psFramesData);

	explicit PSFrames(std::span<const std::byte> psFramesData);

	explicit PSFrames(const std::filesystem::path& psFramesPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getFrameCount() const;

	[[nodiscard]] uint32_t getFPS() const;

	[[nodiscard]] uint16_t getWidth(uint32_t frame) const;

	[[nodiscard]] uint16_t getHeight(uint32_t frame) const;

	[[nodiscard]] std::span<const std::byte> getPaletteDataRaw(uint32_t frame) const;

	[[nodiscard]] std::span<std::byte> getPaletteDataRaw(uint32_t frame);

	[[nodiscard]] std::vector<std::byte> getPaletteDataAs(ImageFormat newFormat, uint32_t frame) const;

	[[nodiscard]] std::span<const std::byte> getImageDataRaw(uint32_t frame) const;

	[[nodiscard]] std::span<std::byte> getImageDataRaw(uint32_t frame);

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint32_t frame) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsBGR888(uint32_t frame) const;

protected:
	bool opened;

	uint32_t frameCount = 0;
	uint32_t fps = 0;

	std::vector<std::byte> data;
	std::vector<Frame> frames;
};

} // namespace vtfpp
