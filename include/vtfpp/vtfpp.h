#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include <sourcepp/math/Vector.h>
#include <sourcepp/parser/Binary.h>
#include <sourcepp/Macros.h>

#include "ImageFormats.h"

namespace vtfpp {

constexpr uint32_t VTF_SIGNATURE = sourcepp::parser::binary::makeFourCC("VTF\0");

struct Resource {
	enum Type : uint8_t {
		TYPE_UNKNOWN             = '\0',   // Unknown
		TYPE_THUMBNAIL_DATA      = '\x01', // \x01\0\0
		TYPE_IMAGE_DATA          = '\x30', // \x30\0\0
		TYPE_PARTICLE_SHEET_DATA = '\x10', // \x10\0\0
		TYPE_CRC                 = 'C',    // CRC
		TYPE_LOD_CONTROL_INFO    = 'L',    // LOD
		TYPE_EXTENDED_FLAGS      = 'T',    // TSO
		TYPE_KEYVALUES_DATA      = 'K',    // KVD
		TYPE_AUX_COMPRESSION     = 'A',    // AXC
	};

	enum Flags : uint8_t {
		FLAG_NONE    = 0,
		FLAG_NO_DATA = 1 << 1,
	};

	Type type = TYPE_UNKNOWN;
	Flags flags = FLAG_NONE;
	std::span<std::byte> data;

	using ConvertedData = std::variant<
		std::monostate, // Anything that would be equivalent to just returning data directly, or used as an error
		uint32_t, // CRC, TSO
		std::pair<uint8_t, uint8_t>, // LOD
		std::string, // KVD
		std::span<uint32_t> // AXC
	>;
	[[nodiscard]] ConvertedData convertData() const;

	[[nodiscard]] uint32_t getDataAsCRC() const {
		return std::get<uint32_t>(this->convertData());
	}

	[[nodiscard]] uint32_t getDataAsExtendedFlags() const {
		return std::get<uint32_t>(this->convertData());
	}

	[[nodiscard]] std::pair<uint8_t, uint8_t> getDataAsLODControlInfo() const {
		return std::get<std::pair<uint8_t, uint8_t>>(this->convertData());
	}

	[[nodiscard]] std::string getDataAsKeyValuesData() const {
		return std::get<std::string>(this->convertData());
	}

	[[nodiscard]] int32_t getDataAsAuxCompressionLevel() const {
		return static_cast<int32_t>(std::get<std::span<uint32_t>>(this->convertData())[1]);
	}

	[[nodiscard]] uint32_t getDataAsAuxCompressionLength(uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount) const {
		return std::get<std::span<uint32_t>>(this->convertData())[((mipCount - 1 - mip) * frameCount * faceCount + frame * faceCount + face) + 2];
	}
};
SOURCEPP_BITWISE_ENUM(Resource::Flags)

class VTF {
public:
	enum Flags : int32_t {
		FLAG_NONE                                    = 0,
		FLAG_POINT_SAMPLE                            = 1 <<  0,
		FLAG_TRILINEAR                               = 1 <<  1,
		FLAG_CLAMP_S                                 = 1 <<  2,
		FLAG_CLAMP_T                                 = 1 <<  3,
		FLAG_ANISOTROPIC                             = 1 <<  4,
		FLAG_HINT_DXT5                               = 1 <<  5,
		FLAG_SRGB                                    = 1 <<  6,
		FLAG_NO_COMPRESS                             = FLAG_SRGB, // Internal to vtex, removed
		FLAG_NORMAL                                  = 1 <<  7,
		FLAG_NO_MIP                                  = 1 <<  8,
		FLAG_NO_LOD                                  = 1 <<  9,
		FLAG_MIN_MIP                                 = 1 << 10,
		FLAG_PROCEDURAL                              = 1 << 11,
		FLAG_ONE_BIT_ALPHA                           = 1 << 12, // Added at VTF creation time
		FLAG_MULTI_BIT_ALPHA                         = 1 << 13, // Added at VTF creation time
		FLAG_ENVMAP                                  = 1 << 14,
		FLAG_RENDERTARGET                            = 1 << 15,
		FLAG_DEPTH_RENDERTARGET                      = 1 << 16,
		FLAG_NO_DEBUG_OVERRIDE                       = 1 << 17,
		FLAG_SINGLE_COPY                             = 1 << 18,
		FLAG_ONE_OVER_MIP_LEVEL_IN_ALPHA             = 1 << 19, // Internal to vtex, removed
		FLAG_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL = 1 << 20, // Internal to vtex, removed
		FLAG_NORMAL_TO_DUDV                          = 1 << 21, // Internal to vtex, removed
		FLAG_ALPHA_TEST_MIP_GENERATION               = 1 << 22,
		FLAG_NO_DEPTH_BUFFER                         = 1 << 23,
		FLAG_NICE_FILTERED                           = 1 << 24, // Internal to vtex, removed
		FLAG_CLAMP_U                                 = 1 << 25,
		FLAG_VERTEX_TEXTURE                          = 1 << 26,
		FLAG_SSBUMP                                  = 1 << 27,
		FLAG_UNFILTERABLE_OK                         = 1 << 28, // Removed
		FLAG_BORDER                                  = 1 << 29,
		FLAG_SPECVAR_RED                             = 1 << 30, // Removed
		FLAG_SPECVAR_ALPHA                           = 1 << 31, // Removed
	};

	explicit VTF(std::vector<std::byte>&& vtfData, bool parseHeaderOnly = false);

	explicit VTF(const std::vector<std::byte>& vtfData, bool parseHeaderOnly = false);

	explicit VTF(const std::vector<unsigned char>& vtfData, bool parseHeaderOnly = false);

	VTF(const std::byte* vtfData, std::size_t vtfSize, bool parseHeaderOnly = false);

	VTF(const unsigned char* vtfData, std::size_t vtfSize, bool parseHeaderOnly = false);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getMajorVersion() const;

	[[nodiscard]] uint32_t getMinorVersion() const;

	[[nodiscard]] uint16_t getWidth(uint8_t mip = 0) const;

	[[nodiscard]] uint16_t getHeight(uint8_t mip = 0) const;

	[[nodiscard]] Flags getFlags() const;

	[[nodiscard]] ImageFormat getFormat() const;

	[[nodiscard]] uint8_t getMipCount() const;

	[[nodiscard]] uint16_t getFrameCount() const;

	[[nodiscard]] uint16_t getFaceCount() const;

	[[nodiscard]] uint16_t getSliceCount() const;

	[[nodiscard]] uint16_t getStartFrame() const;

	[[nodiscard]] sourcepp::math::Vec3f getReflectivity() const;

	[[nodiscard]] float getBumpMapScale() const;

	[[nodiscard]] ImageFormat getThumbnailFormat() const;

	[[nodiscard]] uint8_t getThumbnailWidth() const;

	[[nodiscard]] uint8_t getThumbnailHeight() const;

	[[nodiscard]] const std::vector<std::unique_ptr<Resource>>& getResources() const;

	[[nodiscard]] const Resource* getResource(Resource::Type type) const;

	[[nodiscard]] std::span<const std::byte> getImageDataRaw(uint8_t mip = 0, uint16_t frame = 0, uint16_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint16_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsRGBA8888(uint8_t mip = 0, uint16_t frame = 0, uint16_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> convertAndSaveImageDataToFile(uint8_t mip = 0, uint16_t frame = 0, uint16_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::span<const std::byte> getThumbnailDataRaw() const;

	[[nodiscard]] std::vector<std::byte> getThumbnailDataAs(ImageFormat newFormat) const;

	[[nodiscard]] std::vector<std::byte> getThumbnailDataAsRGBA8888() const;

	[[nodiscard]] std::vector<std::byte> convertAndSaveThumbnailDataToFile() const;

private:
	bool opened = false;

	std::vector<std::byte> data;

	//uint32_t signature;
	uint32_t majorVersion{};
	uint32_t minorVersion{};

	//uint32_t headerSize;

	uint16_t width{};
	uint16_t height{};
	Flags flags{};

	uint16_t frameCount{};
	uint16_t startFrame{};

	//uint8_t _padding0[4];
	sourcepp::math::Vec3f reflectivity{};
	//uint8_t _padding1[4];

	float bumpMapScale{};
	ImageFormat format{};
	uint8_t mipCount{};

	ImageFormat thumbnailFormat{};
	uint8_t thumbnailWidth{};
	uint8_t thumbnailHeight{};

	// 1 for v7.1 and lower
	uint16_t sliceCount{};
	//uint8_t _padding2[3];

	// Technically added in v7.3, but we use it to store image and thumbnail data in v7.2 and lower anyway
	//uint32_t resourceCount;
	std::vector<std::unique_ptr<Resource>> resources;
	//uint8_t _padding3[4];

	// False for v7.5 and lower (not actually a field in the header, just simplifies the check)
	bool hasAuxCompression = false;
};
SOURCEPP_BITWISE_ENUM(VTF::Flags)

} // namespace vtfpp
