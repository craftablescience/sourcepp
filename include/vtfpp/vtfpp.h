#pragma once

#include <cstddef>
#include <vector>

#include <sourcepp/math/Vector.h>

#include "structs/Enums.h"

namespace vtfpp {

constexpr uint32_t VTF_SIGNATURE = 'V' + ('T' << 8) + ('F' << 16);

struct Resource {
	ResourceType type;
	ResourceFlag flags;
	std::vector<std::byte> data;
};

struct VTFOptions {
	/// Only parse file metadata. Resources will also not be parsed
	bool parseHeaderOnly;
};

class VTF {
public:
	VTF(const std::byte* vtfData, std::size_t vtfSize, const VTFOptions& options);

	VTF(const unsigned char* vtfData, std::size_t vtfSize, const VTFOptions& options);

	VTF(const std::vector<std::byte>& vtfData, const VTFOptions& options);

	VTF(const std::vector<unsigned char>& vtfData, const VTFOptions& options);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getMajorVersion() const;

	[[nodiscard]] uint32_t getMinorVersion() const;

	[[nodiscard]] uint16_t getWidth() const;

	[[nodiscard]] uint16_t getHeight() const;

	[[nodiscard]] VTFFlag getFlags() const;

	[[nodiscard]] uint16_t getFrameCount() const;

	[[nodiscard]] uint16_t getStartFrame() const;

	[[nodiscard]] sourcepp::Vec3f getReflectivity() const;

	[[nodiscard]] float getBumpMapScale() const;

	[[nodiscard]] ImageFormat getFormat() const;

	[[nodiscard]] uint8_t getMipCount() const;

	[[nodiscard]] ImageFormat getThumbnailFormat() const;

	[[nodiscard]] uint8_t getThumbnailWidth() const;

	[[nodiscard]] uint8_t getThumbnailHeight() const;

	[[nodiscard]] uint16_t getSliceCount() const;

	[[nodiscard]] const std::vector<Resource>& getResources() const;

private:
	bool opened = false;

	//uint32_t signature;
	uint32_t majorVersion{};
	uint32_t minorVersion{};

	//uint32_t headerSize;

	uint16_t width{};
	uint16_t height{};
	VTFFlag flags{};

	uint16_t frameCount{};
	uint16_t startFrame{};

	//uint8_t _padding0[4];
	sourcepp::Vec3f reflectivity{};
	//uint8_t _padding1[4];

	float bumpMapScale{};
	ImageFormat format{};
	uint8_t mipCount{};

	ImageFormat thumbnailFormat{};
	uint8_t thumbnailWidth{};
	uint8_t thumbnailHeight{};

	// 1 for v7.1 or less
	uint16_t sliceCount{};
	//uint8_t _padding2[3];

	// Technically added in v7.3, but we can use it to store image and thumbnail data in v7.2 and lower anyway
	//uint32_t resourceCount;
	std::vector<Resource> resources;
	//uint8_t _padding3[4];
};

} // namespace vtfpp
