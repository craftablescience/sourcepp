#pragma once

#include <cstddef>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include <sourcepp/math/Vector.h>

#include "structs/ImageFormats.h"

namespace vtfpp {

constexpr uint32_t VTF_SIGNATURE = 'V' + ('T' << 8) + ('F' << 16);

struct Resource {
	enum Type : uint8_t {
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

	Type type;
	Flags flags;
	std::vector<std::byte> data;

	using ConvertedData = std::variant<
			std::monostate, // Anything that would be equivalent to just returning data directly, or used as an error
			uint32_t, // CRC, TSO
			std::pair<uint8_t, uint8_t>, // LOD
			std::string_view // KVD
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

	[[nodiscard]] std::string_view getDataAsKeyValuesData() const {
		return std::get<std::string_view>(this->convertData());
	}
};

struct VTFOptions {
	/// Only parse file metadata. Resources will also not be parsed
	bool parseHeaderOnly;
};

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

	VTF(const std::byte* vtfData, std::size_t vtfSize, const VTFOptions& options);

	VTF(const unsigned char* vtfData, std::size_t vtfSize, const VTFOptions& options);

	VTF(const std::vector<std::byte>& vtfData, const VTFOptions& options);

	VTF(const std::vector<unsigned char>& vtfData, const VTFOptions& options);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getMajorVersion() const;

	[[nodiscard]] uint32_t getMinorVersion() const;

	[[nodiscard]] uint16_t getWidth() const;

	[[nodiscard]] uint16_t getHeight() const;

	[[nodiscard]] Flags getFlags() const;

	[[nodiscard]] ImageFormat getFormat() const;

	[[nodiscard]] uint8_t getMipCount() const;

	[[nodiscard]] uint16_t getFrameCount() const;

	[[nodiscard]] uint16_t getFaceCount() const;

	[[nodiscard]] uint16_t getSliceCount() const;

	[[nodiscard]] uint16_t getStartFrame() const;

	[[nodiscard]] sourcepp::Vec3f getReflectivity() const;

	[[nodiscard]] float getBumpMapScale() const;

	[[nodiscard]] ImageFormat getThumbnailFormat() const;

	[[nodiscard]] uint8_t getThumbnailWidth() const;

	[[nodiscard]] uint8_t getThumbnailHeight() const;

	[[nodiscard]] const std::vector<Resource>& getResources() const;

	[[nodiscard]] const Resource* getResource(Resource::Type type) const;

private:
	bool opened = false;

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
