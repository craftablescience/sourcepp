#pragma once

#include <filesystem>
#include <span>
#include <vector>

#include <vtfpp/ImageFormats.h>

namespace vtfpp {

/// Parses "frames" files as seen in The Orange Box PS3 port, in the TF/MOVIES.GRP file group.
class PSFrames {
public:
	explicit PSFrames(std::vector<std::byte>&& psFramesData);

	explicit PSFrames(std::span<const std::byte> psFramesData);

	explicit PSFrames(const std::filesystem::path& psFramesPath);

	[[nodiscard]] uint32_t getFrameCount() const;

	[[nodiscard]] uint32_t getFPS() const;

	[[nodiscard]] uint16_t getWidth() const;

	[[nodiscard]] uint16_t getHeight() const;

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint32_t frame) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsBGR888(uint32_t frame) const;

protected:
	[[nodiscard]] uint32_t getFrameOffset(uint32_t frame) const;

	[[nodiscard]] uint32_t getFramePaletteAndImageSize() const;

	uint32_t frameCount = 0;
	uint32_t fps = 0;
	uint16_t width = 0;
	uint16_t height = 0;

	std::vector<std::byte> data;
};

} // namespace vtfpp
