#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include <sourcepp/parser/Binary.h>
#include <sourcepp/Macros.h>

#include "HOT.h"
#include "ImageConversion.h"
#include "SHT.h"

namespace vtfpp {

constexpr uint32_t VTF_SIGNATURE = sourcepp::parser::binary::makeFourCC("VTF\0");
constexpr uint32_t XTF_SIGNATURE = sourcepp::parser::binary::makeFourCC("XTF\0");
constexpr uint32_t VTFX_SIGNATURE = sourcepp::parser::binary::makeFourCC("VTFX");
constexpr uint32_t VTF3_SIGNATURE = sourcepp::parser::binary::makeFourCC("VTF3");

enum class CompressionMethod : int16_t {
	// Strata Source v7.6 defines
	DEFLATE = 8,
	ZSTD = 93,

	// Signify the image resource should be compressed with LZMA on console
	CONSOLE_LZMA = 0x360,
};

struct Resource {
	enum Type : uint32_t {
		TYPE_UNKNOWN             = 0, // Unknown
		TYPE_THUMBNAIL_DATA      = 1,
		TYPE_PALETTE_DATA        = 2, // Hack for XBOX platform
		TYPE_FALLBACK_DATA       = 3, // Hack for XBOX platform
		TYPE_PARTICLE_SHEET_DATA = 16,
		TYPE_HOTSPOT_DATA        = 43,
		TYPE_IMAGE_DATA          = 48,
		TYPE_EXTENDED_FLAGS      = sourcepp::parser::binary::makeFourCC("TS0\0"),
		TYPE_CRC                 = sourcepp::parser::binary::makeFourCC("CRC\0"),
		TYPE_AUX_COMPRESSION     = sourcepp::parser::binary::makeFourCC("AXC\0"),
		TYPE_LOD_CONTROL_INFO    = sourcepp::parser::binary::makeFourCC("LOD\0"),
		TYPE_KEYVALUES_DATA      = sourcepp::parser::binary::makeFourCC("KVD\0"),
	};
	static consteval std::array<Type, 11> getOrder() {
		return {
			TYPE_THUMBNAIL_DATA,
			TYPE_PALETTE_DATA,
			TYPE_FALLBACK_DATA,
			TYPE_PARTICLE_SHEET_DATA,
			TYPE_HOTSPOT_DATA,
			// regular Source can't handle out of order resources, but Strata can,
			// and it's the only branch that can read this and 7.6.
			// Put this before the image data to fix resources being cut off when skipping mips
			TYPE_AUX_COMPRESSION,
			TYPE_IMAGE_DATA,
			TYPE_EXTENDED_FLAGS,
			TYPE_CRC,
			TYPE_LOD_CONTROL_INFO,
			TYPE_KEYVALUES_DATA,
		};
	}

	enum Flags : uint8_t {
		FLAG_NONE       = 0,
		FLAG_LOCAL_DATA = 1 << 1,
	};

	Type type = TYPE_UNKNOWN;
	Flags flags = FLAG_NONE;
	std::span<std::byte> data;

	using ConvertedData = std::variant<
		std::monostate, // Anything that would be equivalent to just returning data directly, or used as an error
		SHT, // Particle Sheet
		uint32_t, // CRC, TS0
		std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>, // LOD
		std::string, // KVD
		HOT // Hotspot data
	>;
	[[nodiscard]] ConvertedData convertData() const;

	[[nodiscard]] std::vector<std::byte> getDataAsPalette(uint16_t frame) const {
		static constexpr auto PALETTE_FRAME_SIZE = 256 * sizeof(ImagePixel::BGRA8888);
		if (this->data.size() % PALETTE_FRAME_SIZE != 0 || PALETTE_FRAME_SIZE * frame > this->data.size()) {
			return {};
		}
		return {this->data.data() + PALETTE_FRAME_SIZE * frame, this->data.data() + PALETTE_FRAME_SIZE * (frame + 1)};
	}

	[[nodiscard]] SHT getDataAsParticleSheet() const {
		return std::get<SHT>(this->convertData());
	}

	[[nodiscard]] uint32_t getDataAsCRC() const {
		return std::get<uint32_t>(this->convertData());
	}

	[[nodiscard]] uint32_t getDataAsExtendedFlags() const {
		return std::get<uint32_t>(this->convertData());
	}

	[[nodiscard]] std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> getDataAsLODControlInfo() const {
		return std::get<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>(this->convertData());
	}

	[[nodiscard]] std::string getDataAsKeyValuesData() const {
		return std::get<std::string>(this->convertData());
	}

	[[nodiscard]] HOT getDataAsHotspotData() const {
		return std::get<HOT>(this->convertData());
	}

	[[nodiscard]] int16_t getDataAsAuxCompressionLevel() const {
		if (this->data.size() < sizeof(uint32_t) * 2) {
			return 0;
		}
		return static_cast<int16_t>(BufferStream{this->data}.skip<uint32_t>().read<uint32_t>() & 0xffff);
	}

	[[nodiscard]] CompressionMethod getDataAsAuxCompressionMethod() const {
		if (this->data.size() < sizeof(uint32_t) * 2) {
			return CompressionMethod::DEFLATE;
		}
		const auto method = static_cast<int16_t>((BufferStream{this->data}.skip<uint32_t>().read<uint32_t>() & 0xffff0000) >> 16);
		if (method <= 0) {
			return CompressionMethod::DEFLATE;
		}
		return static_cast<CompressionMethod>(method);
	}

	[[nodiscard]] uint32_t getDataAsAuxCompressionLength(uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount) const {
		if (this->data.size() < ((mipCount - 1 - mip) * frameCount * faceCount + frame * faceCount + face + 2) * sizeof(uint32_t)) {
			return 0;
		}
		return BufferStream{this->data}.skip<uint32_t>((mipCount - 1 - mip) * frameCount * faceCount + frame * faceCount + face + 2).read<uint32_t>();
	}
};
SOURCEPP_BITFLAGS_ENUM(Resource::Flags)

/*
 * ===  EASY DIFFICULTY WRITER API  ===
 *
 * Use a static helper function to create a VTF in one function call - VTF::create
 *
 * === MEDIUM DIFFICULTY WRITER API ===
 *
 * Constructing a VTF instance from existing VTF data will let you modify that data.
 *
 * When constructing a VTF instance from scratch, this class has methods that should be
 * called in a particular order. If they aren't your output VTF will look incorrect or
 * have heavy artifacting. Some of these steps are optional, but the steps actually taken
 * should be done in this order. Reference VTF::create and VTF::createInternal to see the
 * intended "order of operations".
 */
class VTF {
public:
	enum Flags : uint32_t {
		FLAG_V0_POINT_SAMPLE     = 1u <<  0,
		FLAG_V0_TRILINEAR        = 1u <<  1,
		FLAG_V0_CLAMP_S          = 1u <<  2,
		FLAG_V0_CLAMP_T          = 1u <<  3,
		FLAG_V0_ANISOTROPIC      = 1u <<  4,
		FLAG_V0_VTEX_HINT_DXT5   = 1u <<  5, // Internal to vtex
		FLAG_V0_VTEX_NO_COMPRESS = 1u <<  6, // Internal to vtex
		FLAG_V0_NORMAL           = 1u <<  7,
		FLAG_V0_NO_MIP           = 1u <<  8, // Controlled by mip count
		FLAG_V0_NO_LOD           = 1u <<  9,
		FLAG_V0_LOAD_SMALL_MIPS  = 1u << 10,
		FLAG_V0_PROCEDURAL       = 1u << 11,
		FLAG_V0_ONE_BIT_ALPHA    = 1u << 12,
		FLAG_V0_MULTI_BIT_ALPHA  = 1u << 13,
		FLAG_V0_ENVMAP           = 1u << 14, // Controlled by face count
		FLAG_MASK_V0 = FLAG_V0_POINT_SAMPLE | FLAG_V0_TRILINEAR | FLAG_V0_CLAMP_S | FLAG_V0_CLAMP_T | FLAG_V0_ANISOTROPIC | FLAG_V0_VTEX_HINT_DXT5 | FLAG_V0_VTEX_NO_COMPRESS | FLAG_V0_NORMAL | FLAG_V0_NO_MIP | FLAG_V0_NO_LOD | FLAG_V0_LOAD_SMALL_MIPS | FLAG_V0_PROCEDURAL | FLAG_V0_ONE_BIT_ALPHA | FLAG_V0_MULTI_BIT_ALPHA | FLAG_V0_ENVMAP,
		FLAG_MASK_V0_VTEX = FLAG_V0_VTEX_HINT_DXT5 | FLAG_V0_VTEX_NO_COMPRESS,

		FLAG_V1_RENDERTARGET                                 = 1u << 15,
		FLAG_V1_DEPTH_RENDERTARGET                           = 1u << 16,
		FLAG_V1_NO_DEBUG_OVERRIDE                            = 1u << 17,
		FLAG_V1_SINGLE_COPY                                  = 1u << 18,
		FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA             = 1u << 19, // Internal to vtex
		FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL = 1u << 20, // Internal to vtex
		FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV                  = 1u << 21, // Internal to vtex
		FLAG_MASK_V1 = FLAG_V1_RENDERTARGET | FLAG_V1_DEPTH_RENDERTARGET | FLAG_V1_NO_DEBUG_OVERRIDE | FLAG_V1_SINGLE_COPY | FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA | FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL | FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV,
		FLAG_MASK_V1_VTEX = FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA | FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL | FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV,

		FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION = 1u << 22, // Internal to vtex
		FLAG_V2_NO_DEPTH_BUFFER                = 1u << 23,
		FLAG_V2_VTEX_NICE_FILTERED             = 1u << 24, // Internal to vtex
		FLAG_V2_CLAMP_U                        = 1u << 25,
		FLAG_MASK_V2 = FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION | FLAG_V2_NO_DEPTH_BUFFER | FLAG_V2_VTEX_NICE_FILTERED | FLAG_V2_CLAMP_U,
		FLAG_MASK_V2_VTEX = FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION | FLAG_V2_VTEX_NICE_FILTERED,

		FLAG_XBOX_VTEX_PRESWIZZLED = 1u << 26,
		FLAG_XBOX_CACHEABLE        = 1u << 27,
		FLAG_XBOX_UNFILTERABLE_OK  = 1u << 28,
		FLAG_MASK_XBOX = FLAG_XBOX_VTEX_PRESWIZZLED | FLAG_XBOX_CACHEABLE | FLAG_XBOX_UNFILTERABLE_OK,
		FLAG_MASK_XBOX_VTEX = FLAG_XBOX_VTEX_PRESWIZZLED,

		FLAG_V3_LOAD_ALL_MIPS  = 1u << 10,
		FLAG_V3_VERTEX_TEXTURE = 1u << 26,
		FLAG_V3_SSBUMP         = 1u << 27,
		FLAG_V3_BORDER         = 1u << 29,
		FLAG_MASK_V3 = FLAG_V3_LOAD_ALL_MIPS | FLAG_V3_VERTEX_TEXTURE | FLAG_V3_SSBUMP | FLAG_V3_BORDER,

		FLAG_V4_SRGB = 1u << 6,
		FLAG_MASK_V4 = FLAG_V4_SRGB,

		FLAG_V4_TF2_STAGING_MEMORY    = 1u << 19,
		FLAG_V4_TF2_IMMEDIATE_CLEANUP = 1u << 20,
		FLAG_V4_TF2_IGNORE_PICMIP     = 1u << 21,
		FLAG_V4_TF2_STREAMABLE_COARSE = 1u << 30,
		FLAG_V4_TF2_STREAMABLE_FINE   = 1u << 31,
		FLAG_MASK_V4_TF2 = FLAG_V4_TF2_STAGING_MEMORY | FLAG_V4_TF2_IMMEDIATE_CLEANUP | FLAG_V4_TF2_IGNORE_PICMIP | FLAG_V4_TF2_STREAMABLE_COARSE | FLAG_V4_TF2_STREAMABLE_FINE,

		FLAG_V5_PWL_CORRECTED  = 1u <<  6,
		FLAG_V5_SRGB           = 1u << 19,
		FLAG_V5_DEFAULT_POOL   = 1u << 20,
		FLAG_V5_LOAD_MOST_MIPS = 1u << 28,
		FLAG_MASK_V5 = FLAG_V5_PWL_CORRECTED | FLAG_V5_SRGB | FLAG_V5_DEFAULT_POOL | FLAG_V5_LOAD_MOST_MIPS,

		FLAG_V5_CSGO_COMBINED                   = 1u << 21,
		FLAG_V5_CSGO_ASYNC_DOWNLOAD             = 1u << 22,
		FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD      = 1u << 24,
		FLAG_V5_CSGO_YCOCG                      = 1u << 30,
		FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES = 1u << 31,
		FLAG_MASK_V5_CSGO = FLAG_V5_CSGO_COMBINED | FLAG_V5_CSGO_ASYNC_DOWNLOAD | FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD | FLAG_V5_CSGO_YCOCG | FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES,

		FLAG_MASK_INTERNAL = FLAG_V0_NO_MIP | FLAG_V0_ENVMAP,
	};

	enum Platform : uint32_t {
		PLATFORM_UNKNOWN       = 0x000,
		PLATFORM_PC            = 0x007,
		PLATFORM_XBOX          = 0x005,
		PLATFORM_X360          = 0x360,
		PLATFORM_PS3_ORANGEBOX = 0x333,
		PLATFORM_PS3_PORTAL2   = 0x334,
	};

	struct CreationOptions {
		uint32_t version = 4;
		ImageFormat outputFormat = FORMAT_DEFAULT;
		float compressedFormatQuality = ImageConversion::DEFAULT_COMPRESSED_QUALITY;
		ImageConversion::ResizeMethod widthResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeMethod heightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT;
		uint32_t flags = 0;
		uint16_t initialFrameCount = 1;
		uint16_t startFrame = 0;
		bool isCubeMap = false;
		uint16_t initialDepth = 1;
		bool computeTransparencyFlags = true;
		bool computeMips = true;
		bool computeThumbnail = true;
		bool computeReflectivity = true;
		Platform platform = PLATFORM_PC;
		int16_t compressionLevel = -1;
		CompressionMethod compressionMethod = CompressionMethod::ZSTD;
		float bumpMapScale = 1.f;
		float gammaCorrection = 1.f;
		bool invertGreenChannel = false;
		uint8_t consoleMipScale = 0;
	};

	/// This value is only valid when passed to VTF::create through CreationOptions
	static constexpr auto FORMAT_UNCHANGED = static_cast<ImageFormat>(-2);

	/// This value is only valid when passed to VTF::create through CreationOptions or VTF::setFormat
	static constexpr auto FORMAT_DEFAULT = static_cast<ImageFormat>(-1);

	VTF();

	explicit VTF(std::vector<std::byte>&& vtfData, bool parseHeaderOnly = false);

	explicit VTF(std::span<const std::byte> vtfData, bool parseHeaderOnly = false);

	explicit VTF(const std::string& vtfPath, bool parseHeaderOnly = false);

	VTF(const VTF& other);

	VTF& operator=(const VTF& other);

	VTF(VTF&&) noexcept = default;

	VTF& operator=(VTF&&) noexcept = default;

	[[nodiscard]] explicit operator bool() const;

	static bool create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, const CreationOptions& options);

	static bool create(ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, const CreationOptions& options);

	[[nodiscard]] static VTF create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options);

	[[nodiscard]] static VTF create(ImageFormat format, uint16_t width, uint16_t height, const CreationOptions& options);

	static bool create(const std::string& imagePath, const std::string& vtfPath, const CreationOptions& options);

	[[nodiscard]] static VTF create(const std::string& imagePath, const CreationOptions& options);

	[[nodiscard]] Platform getPlatform() const;

	void setPlatform(Platform newPlatform);

	[[nodiscard]] uint32_t getVersion() const;

	void setVersion(uint32_t newVersion);

	[[nodiscard]] ImageConversion::ResizeMethod getImageWidthResizeMethod() const;

	[[nodiscard]] ImageConversion::ResizeMethod getImageHeightResizeMethod() const;

	void setImageResizeMethods(ImageConversion::ResizeMethod imageWidthResizeMethod_, ImageConversion::ResizeMethod imageHeightResizeMethod_);

	void setImageWidthResizeMethod(ImageConversion::ResizeMethod imageWidthResizeMethod_);

	void setImageHeightResizeMethod(ImageConversion::ResizeMethod imageHeightResizeMethod_);

	[[nodiscard]] uint16_t getWidth(uint8_t mip = 0) const;

	[[nodiscard]] uint16_t getHeight(uint8_t mip = 0) const;

	void setSize(uint16_t newWidth, uint16_t newHeight, ImageConversion::ResizeFilter filter);

	[[nodiscard]] uint32_t getFlags() const;

	void setFlags(uint32_t flags_);

	void addFlags(uint32_t flags_);

	void removeFlags(uint32_t flags_);

	[[nodiscard]] bool isSRGB() const;

	void setSRGB(bool srgb);

	void computeTransparencyFlags();

	[[nodiscard]] static ImageFormat getDefaultCompressedFormat(ImageFormat inputFormat, uint32_t version, bool isCubeMap);

	[[nodiscard]] ImageFormat getFormat() const;

	void setFormat(ImageFormat newFormat, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	[[nodiscard]] uint8_t getMipCount() const;

	bool setMipCount(uint8_t newMipCount);

	bool setRecommendedMipCount();

	void computeMips(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	[[nodiscard]] uint16_t getFrameCount() const;

	bool setFrameCount(uint16_t newFrameCount);

	[[nodiscard]] uint8_t getFaceCount() const;

	bool setFaceCount(bool isCubeMap);

	[[nodiscard]] uint16_t getDepth(uint8_t mip = 0) const;

	bool setDepth(uint16_t newDepth);

	bool setFrameFaceAndDepth(uint16_t newFrameCount, bool isCubeMap, uint16_t newDepth = 1);

	[[nodiscard]] uint16_t getStartFrame() const;

	void setStartFrame(uint16_t newStartFrame);

	[[nodiscard]] sourcepp::math::Vec3f getReflectivity() const;

	void setReflectivity(sourcepp::math::Vec3f newReflectivity);

	void computeReflectivity();

	[[nodiscard]] float getBumpMapScale() const;

	void setBumpMapScale(float newBumpMapScale);

	[[nodiscard]] ImageFormat getThumbnailFormat() const;

	[[nodiscard]] uint8_t getThumbnailWidth() const;

	[[nodiscard]] uint8_t getThumbnailHeight() const;

	[[nodiscard]] uint8_t getFallbackWidth() const;

	[[nodiscard]] uint8_t getFallbackHeight() const;

	[[nodiscard]] uint8_t getFallbackMipCount() const;

	[[nodiscard]] const std::vector<Resource>& getResources() const;

	[[nodiscard]] const Resource* getResource(Resource::Type type) const;

	[[nodiscard]] std::vector<std::byte> getPaletteResourceFrame(uint16_t frame = 0) const;

	/// This is a convenience function. You're best off uploading the bounds to the GPU and scaling the UV there if trying to render a particle
	[[nodiscard]] std::vector<std::byte> getParticleSheetFrameDataRaw(uint16_t& spriteWidth, uint16_t& spriteHeight, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	/// This is a convenience function. You're best off uploading the bounds to the GPU and scaling the UV there if trying to render a particle
	[[nodiscard]] std::vector<std::byte> getParticleSheetFrameDataAs(ImageFormat newFormat, uint16_t& spriteWidth, uint16_t& spriteHeight, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	/// This is a convenience function. You're best off uploading the bounds to the GPU and scaling the UV there if trying to render a particle
	[[nodiscard]] std::vector<std::byte> getParticleSheetFrameDataAsRGBA8888(uint16_t& spriteWidth, uint16_t& spriteHeight, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds = 0, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	void setParticleSheetResource(const SHT& value);

	void removeParticleSheetResource();

	void setCRCResource(uint32_t value);

	void removeCRCResource();

	void setLODResource(uint8_t u, uint8_t v, uint8_t u360 = 0, uint8_t v360 = 0);

	void removeLODResource();

	void setExtendedFlagsResource(uint32_t value);

	void removeExtendedFlagsResource();

	void setKeyValuesDataResource(const std::string& value);

	void removeKeyValuesDataResource();

	void setHotspotDataResource(const HOT& value);

	void removeHotspotDataResource();

	[[nodiscard]] int16_t getCompressionLevel() const;

	void setCompressionLevel(int16_t newCompressionLevel);

	[[nodiscard]] CompressionMethod getCompressionMethod() const;

	void setCompressionMethod(CompressionMethod newCompressionMethod);

	[[nodiscard]] bool hasImageData() const;

	[[nodiscard]] std::span<const std::byte> getImageDataRaw(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsRGBA8888(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	bool setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	bool setImage(const std::string& imagePath, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	[[nodiscard]] std::vector<std::byte> saveImageToFile(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;

	bool saveImageToFile(const std::string& imagePath, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)

	[[nodiscard]] bool hasThumbnailData() const;

	[[nodiscard]] std::span<const std::byte> getThumbnailDataRaw() const;

	[[nodiscard]] std::vector<std::byte> getThumbnailDataAs(ImageFormat newFormat) const;

	[[nodiscard]] std::vector<std::byte> getThumbnailDataAsRGBA8888() const;

	void setThumbnail(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	bool setThumbnail(const std::string& imagePath, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY); // NOLINT(*-use-nodiscard)

	void computeThumbnail(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	void removeThumbnail();

	[[nodiscard]] std::vector<std::byte> saveThumbnailToFile(ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;

	bool saveThumbnailToFile(const std::string& imagePath, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)

	[[nodiscard]] bool hasFallbackData() const;

	[[nodiscard]] std::span<const std::byte> getFallbackDataRaw(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) const;

	[[nodiscard]] std::vector<std::byte> getFallbackDataAs(ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) const;

	[[nodiscard]] std::vector<std::byte> getFallbackDataAsRGBA8888(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0) const;

	void computeFallback(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	void removeFallback();

	[[nodiscard]] std::vector<std::byte> saveFallbackToFile(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;

	bool saveFallbackToFile(const std::string& imagePath, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)

	[[nodiscard]] uint8_t getConsoleMipScale() const;

	void setConsoleMipScale(uint8_t consoleMipScale_);

	[[nodiscard]] uint64_t estimateBakeSize() const;

	[[nodiscard]] uint64_t estimateBakeSize(bool& isExact) const;

	[[nodiscard]] std::vector<std::byte> bake() const;

	bool bake(const std::string& vtfPath) const; // NOLINT(*-use-nodiscard)

protected:
	static bool createInternal(VTF& writer, CreationOptions options);

	[[nodiscard]] Resource* getResourceInternal(Resource::Type type);

	void setResourceInternal(Resource::Type type, std::span<const std::byte> data_);

	void removeResourceInternal(Resource::Type type);

	void regenerateImageData(ImageFormat newFormat, uint16_t newWidth, uint16_t newHeight, uint8_t newMipCount, uint16_t newFrameCount, uint8_t newFaceCount, uint16_t newDepth, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, float quality = ImageConversion::DEFAULT_COMPRESSED_QUALITY);

	bool opened = false;

	std::vector<std::byte> data;

	uint32_t version = 4;

	uint16_t width = 0;
	uint16_t height = 0;
	uint32_t flags = FLAG_V0_NO_MIP | FLAG_V0_NO_LOD;

	uint16_t frameCount = 1;
	uint16_t startFrame = 0;

	sourcepp::math::Vec3f reflectivity{0.2f, 0.2f, 0.2f};

	float bumpMapScale = 0.f;
	ImageFormat format = ImageFormat::EMPTY;
	uint8_t mipCount = 1;

	ImageFormat thumbnailFormat = ImageFormat::EMPTY;
	uint8_t thumbnailWidth = 0;
	uint8_t thumbnailHeight = 0;

	uint8_t fallbackWidth = 0;
	uint8_t fallbackHeight = 0;
	uint8_t fallbackMipCount = 0;

	// Number of times to multiply the scale of each mip by 2 when rendering on console
	uint8_t consoleMipScale = 0;

	// 1 for v7.1 and lower
	uint16_t depth = 1;

	// Technically added in v7.3, but we use it to store image and thumbnail data in v7.2 and lower anyway
	std::vector<Resource> resources;

	// These aren't in the header
	Platform platform = PLATFORM_PC;
	int16_t compressionLevel = 0;
	CompressionMethod compressionMethod = CompressionMethod::ZSTD;
	ImageConversion::ResizeMethod imageWidthResizeMethod  = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
	ImageConversion::ResizeMethod imageHeightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
};
SOURCEPP_BITFLAGS_ENUM(VTF::Flags)

} // namespace vtfpp
