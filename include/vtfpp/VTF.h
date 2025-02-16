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

#include "ImageConversion.h"
#include "SHT.h"

namespace vtfpp {

constexpr uint32_t VTF_SIGNATURE = sourcepp::parser::binary::makeFourCC("VTF\0");
constexpr uint32_t VTFX_SIGNATURE = sourcepp::parser::binary::makeFourCC("VTFX");

enum class CompressionMethod : int16_t {
	// Strata Source v7.6 defines
	DEFLATE = 8,
	ZSTD = 93,

	// Signify the image resource should be compressed with LZMA on console
	CONSOLE_LZMA = 0x360,
};

struct Resource {
	enum Type : uint32_t {
		TYPE_UNKNOWN             = 0,   // Unknown
		TYPE_THUMBNAIL_DATA      = sourcepp::parser::binary::makeFourCC("\x01\0\0\0"),
		TYPE_IMAGE_DATA          = sourcepp::parser::binary::makeFourCC("\x30\0\0\0"),
		TYPE_PARTICLE_SHEET_DATA = sourcepp::parser::binary::makeFourCC("\x10\0\0\0"),
		TYPE_CRC                 = sourcepp::parser::binary::makeFourCC("CRC\0"),
		TYPE_LOD_CONTROL_INFO    = sourcepp::parser::binary::makeFourCC("LOD\0"),
		TYPE_EXTENDED_FLAGS      = sourcepp::parser::binary::makeFourCC("TSO\0"),
		TYPE_KEYVALUES_DATA      = sourcepp::parser::binary::makeFourCC("KVD\0"),
		TYPE_AUX_COMPRESSION     = sourcepp::parser::binary::makeFourCC("AXC\0"),
	};
	static const std::array<Type, 8>& getOrder();

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
		uint32_t, // CRC, TSO
		std::pair<uint8_t, uint8_t>, // LOD
		std::string, // KVD
		std::span<uint32_t> // AXC
	>;
	[[nodiscard]] ConvertedData convertData() const;

	[[nodiscard]] SHT getDataAsParticleSheet() const {
		return std::get<SHT>(this->convertData());
	}

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

	[[nodiscard]] int16_t getDataAsAuxCompressionLevel() const {
		return static_cast<int16_t>(std::get<std::span<uint32_t>>(this->convertData())[1] & 0xffff);
	}

	[[nodiscard]] CompressionMethod getDataAsAuxCompressionMethod() const {
		auto method = static_cast<int16_t>((std::get<std::span<uint32_t>>(this->convertData())[1] & 0xffff0000) >> 16);
		if (method <= 0) {
			return CompressionMethod::DEFLATE;
		}
		return static_cast<CompressionMethod>(method);
	}

	[[nodiscard]] uint32_t getDataAsAuxCompressionLength(uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount) const {
		return std::get<std::span<uint32_t>>(this->convertData())[((mipCount - 1 - mip) * frameCount * faceCount + frame * faceCount + face) + 2];
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
 * This class has methods that should be called in a particular order
 * when creating a VTF from scratch, or your output VTF may look incorrect:
 *
 * 0. Construct an empty VTF instance
 * 1. Set the version (7.4 is the default)                 - VTF::setVersion
 * 2. Set FLAG_SRGB (optional, read for image resizing)    - VTF::setFlags
 * 3. Set the image resize methods (optional)              - VTF::setImageResizeMethods
 * 4. Set the base image (mip 0, frame 0, face 0, slice 0) - VTF::setImage
 * 5. Compute mips (optional)                              - VTF::computeMips
 * 6. Set the output format (optional)                     - VTF::setFormat
 *
 * After these methods are called, the other writer methods in the class should work as expected.
 */
class VTF {
public:
	enum Flags : int32_t {
		FLAG_NONE                       = 0,
		FLAG_POINT_SAMPLE               = 1 <<  0,
		FLAG_TRILINEAR                  = 1 <<  1,
		FLAG_CLAMP_S                    = 1 <<  2,
		FLAG_CLAMP_T                    = 1 <<  3,
		FLAG_ANISOTROPIC                = 1 <<  4,
		FLAG_HINT_DXT5                  = 1 <<  5,
		FLAG_PWL_CORRECTED              = 1 <<  6,
		FLAG_NORMAL                     = 1 <<  7,
		FLAG_NO_MIP                     = 1 <<  8, // Applied at VTF bake time
		FLAG_NO_LOD                     = 1 <<  9, // Applied at VTF bake time
		FLAG_LOAD_ALL_MIPS              = 1 << 10,
		FLAG_PROCEDURAL                 = 1 << 11,
		FLAG_ONE_BIT_ALPHA              = 1 << 12, // Applied at VTF bake time
		FLAG_MULTI_BIT_ALPHA            = 1 << 13, // Applied at VTF bake time
		FLAG_ENVMAP                     = 1 << 14, // Applied at VTF bake time
		FLAG_RENDERTARGET               = 1 << 15,
		FLAG_DEPTH_RENDERTARGET         = 1 << 16,
		FLAG_NO_DEBUG_OVERRIDE          = 1 << 17,
		FLAG_SINGLE_COPY                = 1 << 18, // Unused
		FLAG_SRGB                       = 1 << 19,
		FLAG_DEFAULT_POOL               = 1 << 20,
		FLAG_COMBINED                   = 1 << 21,
		FLAG_ASYNC_DOWNLOAD             = 1 << 22,
		FLAG_NO_DEPTH_BUFFER            = 1 << 23,
		FLAG_SKIP_INITIAL_DOWNLOAD      = 1 << 24,
		FLAG_CLAMP_U                    = 1 << 25,
		FLAG_VERTEX_TEXTURE             = 1 << 26,
		FLAG_SSBUMP                     = 1 << 27,
		FLAG_LOAD_MOST_MIPS             = 1 << 28,
		FLAG_BORDER                     = 1 << 29,
		FLAG_YCOCG                      = 1 << 30,
		FLAG_ASYNC_SKIP_INITIAL_LOW_RES = 1 << 31,
	};
	static constexpr std::underlying_type_t<Flags> FLAG_MASK_AFTER_V7_3 = FLAG_LOAD_ALL_MIPS | FLAG_SRGB | FLAG_DEFAULT_POOL | FLAG_COMBINED | FLAG_ASYNC_DOWNLOAD | FLAG_SKIP_INITIAL_DOWNLOAD | FLAG_LOAD_MOST_MIPS | FLAG_YCOCG | FLAG_ASYNC_SKIP_INITIAL_LOW_RES;
	static constexpr std::underlying_type_t<Flags> FLAG_MASK_GENERATED = FLAG_NO_MIP | FLAG_NO_LOD | FLAG_ONE_BIT_ALPHA | FLAG_MULTI_BIT_ALPHA | FLAG_ENVMAP;
	static constexpr std::underlying_type_t<Flags> FLAG_MASK_SRGB = FLAG_PWL_CORRECTED | FLAG_SRGB;

	enum Platform : uint32_t {
		PLATFORM_UNKNOWN = 0,
		PLATFORM_PC      = 1,
		PLATFORM_PS3     = 0x333,
		PLATFORM_X360    = 0x360,
	};

	struct CreationOptions {
		uint32_t majorVersion = 7;
		uint32_t minorVersion = 4;
		ImageFormat outputFormat = FORMAT_DEFAULT;
		ImageConversion::ResizeMethod widthResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeMethod heightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT;
		Flags flags = FLAG_NONE;
		uint16_t initialFrameCount = 1;
		uint16_t startFrame = 0;
		bool isCubeMap = false;
		bool hasSphereMap = false;
		uint16_t initialSliceCount = 1;
		bool computeMips = true;
		bool computeThumbnail = true;
		bool computeReflectivity = true;
		Platform platform = PLATFORM_PC;
		int16_t compressionLevel = -1;
		CompressionMethod compressionMethod = CompressionMethod::ZSTD;
		float bumpMapScale = 1.f;
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

	static bool create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, CreationOptions options);

	static bool create(ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, CreationOptions options);

	[[nodiscard]] static VTF create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, CreationOptions options);

	[[nodiscard]] static VTF create(ImageFormat format, uint16_t width, uint16_t height, CreationOptions options);

	static bool create(const std::string& imagePath, const std::string& vtfPath, CreationOptions options);

	[[nodiscard]] static VTF create(const std::string& imagePath, CreationOptions options);

	[[nodiscard]] Platform getPlatform() const;

	void setPlatform(Platform newPlatform);

	[[nodiscard]] uint32_t getMajorVersion() const;

	[[nodiscard]] uint32_t getMinorVersion() const;

	void setVersion(uint32_t newMajorVersion, uint32_t newMinorVersion);

	void setMajorVersion(uint32_t newMajorVersion);

	void setMinorVersion(uint32_t newMinorVersion);

	[[nodiscard]] ImageConversion::ResizeMethod getImageWidthResizeMethod() const;

	[[nodiscard]] ImageConversion::ResizeMethod getImageHeightResizeMethod() const;

	void setImageResizeMethods(ImageConversion::ResizeMethod imageWidthResizeMethod_, ImageConversion::ResizeMethod imageHeightResizeMethod_);

	void setImageWidthResizeMethod(ImageConversion::ResizeMethod imageWidthResizeMethod_);

	void setImageHeightResizeMethod(ImageConversion::ResizeMethod imageHeightResizeMethod_);

	[[nodiscard]] uint16_t getWidth(uint8_t mip = 0) const;

	[[nodiscard]] uint16_t getHeight(uint8_t mip = 0) const;

	void setSize(uint16_t newWidth, uint16_t newHeight, ImageConversion::ResizeFilter filter);

	[[nodiscard]] Flags getFlags() const;

	void setFlags(Flags flags_);

	void addFlags(Flags flags_);

	void removeFlags(Flags flags_);

	[[nodiscard]] static ImageFormat getDefaultCompressedFormat(ImageFormat inputFormat, uint32_t majorVersion, uint32_t minorVersion);

	[[nodiscard]] ImageFormat getFormat() const;

	void setFormat(ImageFormat newFormat, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	[[nodiscard]] uint8_t getMipCount() const;

	bool setMipCount(uint8_t newMipCount);

	bool setRecommendedMipCount();

	void computeMips(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	[[nodiscard]] uint16_t getFrameCount() const;

	bool setFrameCount(uint16_t newFrameCount);

	[[nodiscard]] uint8_t getFaceCount() const;

	bool setFaceCount(bool isCubemap, bool hasSphereMap = false);

	[[nodiscard]] uint16_t getSliceCount() const;

	bool setSliceCount(uint16_t newSliceCount);

	bool setFrameFaceAndSliceCount(uint16_t newFrameCount, bool isCubemap, bool hasSphereMap = false, uint16_t newSliceCount = 1);

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

	[[nodiscard]] const std::vector<Resource>& getResources() const;

	[[nodiscard]] const Resource* getResource(Resource::Type type) const;

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

	void setLODResource(uint8_t u, uint8_t v);

	void removeLODResource();

	void setExtendedFlagsResource(uint32_t value);

	void removeExtendedFlagsResource();

	void setKeyValuesDataResource(const std::string& value);

	void removeKeyValuesDataResource();

	[[nodiscard]] int16_t getCompressionLevel() const;

	void setCompressionLevel(int16_t newCompressionLevel);

	[[nodiscard]] CompressionMethod getCompressionMethod() const;

	void setCompressionMethod(CompressionMethod newCompressionMethod);

	[[nodiscard]] bool hasImageData() const;

	[[nodiscard]] bool imageDataIsSRGB() const;

	[[nodiscard]] std::span<const std::byte> getImageDataRaw(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAs(ImageFormat newFormat, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	[[nodiscard]] std::vector<std::byte> getImageDataAsRGBA8888(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0) const;

	bool setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0);

	bool setImage(const std::string& imagePath, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0);

	[[nodiscard]] std::vector<std::byte> saveImageToFile(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;

	bool saveImageToFile(const std::string& imagePath, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)

	[[nodiscard]] bool hasThumbnailData() const;

	[[nodiscard]] std::span<const std::byte> getThumbnailDataRaw() const;

	[[nodiscard]] std::vector<std::byte> getThumbnailDataAs(ImageFormat newFormat) const;

	[[nodiscard]] std::vector<std::byte> getThumbnailDataAsRGBA8888() const;

	void setThumbnail(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_);

	void computeThumbnail(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	void removeThumbnail();

	[[nodiscard]] std::vector<std::byte> saveThumbnailToFile(ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;

	bool saveThumbnailToFile(const std::string& imagePath, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)

	[[nodiscard]] std::vector<std::byte> bake() const;

	bool bake(const std::string& vtfPath) const; // NOLINT(*-use-nodiscard)

protected:
	static void createInternal(VTF& writer, CreationOptions options);

	[[nodiscard]] Resource* getResourceInternal(Resource::Type type);

	void setResourceInternal(Resource::Type type, std::span<const std::byte> data_);

	void removeResourceInternal(Resource::Type type);

	void regenerateImageData(ImageFormat newFormat, uint16_t newWidth, uint16_t newHeight, uint8_t newMipCount, uint16_t newFrameCount, uint8_t newFaceCount, uint16_t newSliceCount, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	bool opened = false;

	std::vector<std::byte> data;

	//uint32_t signature;
	uint32_t majorVersion{};
	uint32_t minorVersion{};
	//uint32_t headerSize;

	uint16_t width{};
	uint16_t height{};
	Flags flags{};

	uint16_t frameCount = 1;
	uint16_t startFrame{};

	//uint8_t _padding0[4];
	sourcepp::math::Vec3f reflectivity{};
	//uint8_t _padding1[4];

	float bumpMapScale{};
	ImageFormat format = ImageFormat::EMPTY;
	uint8_t mipCount = 1;

	ImageFormat thumbnailFormat = ImageFormat::EMPTY;
	uint8_t thumbnailWidth{};
	uint8_t thumbnailHeight{};

	// 1 for v7.1 and lower
	uint16_t sliceCount = 1;
	//uint8_t _padding2[3];

	// Technically added in v7.3, but we use it to store image and thumbnail data in v7.2 and lower anyway
	//uint32_t resourceCount;
	std::vector<Resource> resources;
	//uint8_t _padding3[4];

	// These aren't in the header
	Platform platform = PLATFORM_PC;
	int16_t compressionLevel = 0;
	CompressionMethod compressionMethod = CompressionMethod::ZSTD;
	ImageConversion::ResizeMethod imageWidthResizeMethod  = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
	ImageConversion::ResizeMethod imageHeightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
};
SOURCEPP_BITFLAGS_ENUM(VTF::Flags)

} // namespace vtfpp
