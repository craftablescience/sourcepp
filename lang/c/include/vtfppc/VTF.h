#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "ImageConversion.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VTFPP_COMPRESSION_METHOD_DEFLATE,
	VTFPP_COMPRESSION_METHOD_ZSTD,
} vtfpp_compression_method_e;

typedef enum {
	VTFPP_RESOURCE_TYPE_UNKNOWN,
	VTFPP_RESOURCE_TYPE_THUMBNAIL_DATA,
	VTFPP_RESOURCE_TYPE_IMAGE_DATA,
	VTFPP_RESOURCE_TYPE_PARTICLE_SHEET_DATA,
	VTFPP_RESOURCE_TYPE_CRC,
	VTFPP_RESOURCE_TYPE_LOD_CONTROL_INFO,
	VTFPP_RESOURCE_TYPE_EXTENDED_FLAGS,
	VTFPP_RESOURCE_TYPE_KEYVALUES_DATA,
	VTFPP_RESOURCE_TYPE_AUX_COMPRESSION,
} vtfpp_resource_type_e;

typedef enum {
	VTFPP_RESOURCE_FLAG_NONE       = 0,
	VTFPP_RESOURCE_FLAG_LOCAL_DATA = 1 << 1,
} vtfpp_resource_flags_e;

typedef void* vtfpp_resource_handle_t;

typedef enum {
	VTFPP_VTF_FLAG_NONE                                    = 0,
	VTFPP_VTF_FLAG_POINT_SAMPLE                            = 1 <<  0,
	VTFPP_VTF_FLAG_TRILINEAR                               = 1 <<  1,
	VTFPP_VTF_FLAG_CLAMP_S                                 = 1 <<  2,
	VTFPP_VTF_FLAG_CLAMP_T                                 = 1 <<  3,
	VTFPP_VTF_FLAG_ANISOTROPIC                             = 1 <<  4,
	VTFPP_VTF_FLAG_HINT_DXT5                               = 1 <<  5,
	VTFPP_VTF_FLAG_SRGB                                    = 1 <<  6,
	VTFPP_VTF_FLAG_NO_COMPRESS                             = VTFPP_VTF_FLAG_SRGB,
	VTFPP_VTF_FLAG_NORMAL                                  = 1 <<  7,
	VTFPP_VTF_FLAG_NO_MIP                                  = 1 <<  8,
	VTFPP_VTF_FLAG_NO_LOD                                  = 1 <<  9,
	VTFPP_VTF_FLAG_LOAD_LOWEST_MIPS                        = 1 << 10,
	VTFPP_VTF_FLAG_PROCEDURAL                              = 1 << 11,
	VTFPP_VTF_FLAG_ONE_BIT_ALPHA                           = 1 << 12,
	VTFPP_VTF_FLAG_MULTI_BIT_ALPHA                         = 1 << 13,
	VTFPP_VTF_FLAG_ENVMAP                                  = 1 << 14,
	VTFPP_VTF_FLAG_RENDERTARGET                            = 1 << 15,
	VTFPP_VTF_FLAG_DEPTH_RENDERTARGET                      = 1 << 16,
	VTFPP_VTF_FLAG_NO_DEBUG_OVERRIDE                       = 1 << 17,
	VTFPP_VTF_FLAG_SINGLE_COPY                             = 1 << 18,
	VTFPP_VTF_FLAG_ONE_OVER_MIP_LEVEL_IN_ALPHA             = 1 << 19,
	VTFPP_VTF_FLAG_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL = 1 << 20,
	VTFPP_VTF_FLAG_NORMAL_TO_DUDV                          = 1 << 21,
	VTFPP_VTF_FLAG_ALPHA_TEST_MIP_GENERATION               = 1 << 22,
	VTFPP_VTF_FLAG_NO_DEPTH_BUFFER                         = 1 << 23,
	VTFPP_VTF_FLAG_NICE_FILTERED                           = 1 << 24,
	VTFPP_VTF_FLAG_CLAMP_U                                 = 1 << 25,
	VTFPP_VTF_FLAG_VERTEX_TEXTURE                          = 1 << 26,
	VTFPP_VTF_FLAG_SSBUMP                                  = 1 << 27,
	VTFPP_VTF_FLAG_UNFILTERABLE_OK                         = 1 << 28,
	VTFPP_VTF_FLAG_BORDER                                  = 1 << 29,
	VTFPP_VTF_FLAG_SPECVAR_RED                             = 1 << 30,
	VTFPP_VTF_FLAG_SPECVAR_ALPHA                           = 1 << 31,
} vtfpp_vtf_flags_e;

#define VTFPP_VTF_FLAGS_MASK_GENERATED (VTF_FLAG_NO_MIP | VTF_FLAG_NO_LOD | VTF_FLAG_ONE_BIT_ALPHA | VTF_FLAG_MULTI_BIT_ALPHA | VTF_FLAG_ENVMAP)

#ifdef __cplusplus
} // extern "C"
#endif

struct Resource {
	static const std::array<Type, 8>& getOrder();

	// add getters
	Type type = TYPE_UNKNOWN;
	Flags flags = FLAG_NONE;
	std::span<std::byte> data;

	[[nodiscard]] SHT getDataAsParticleSheet() const;
	[[nodiscard]] uint32_t getDataAsCRC() const;
	[[nodiscard]] uint32_t getDataAsExtendedFlags() const;
	[[nodiscard]] std::pair<uint8_t, uint8_t> getDataAsLODControlInfo() const;
	[[nodiscard]] std::string getDataAsKeyValuesData() const;
	[[nodiscard]] int16_t getDataAsAuxCompressionLevel() const;
	[[nodiscard]] CompressionMethod getDataAsAuxCompressionMethod() const;
	[[nodiscard]] uint32_t getDataAsAuxCompressionLength(uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount) const;
};

class VTF {
public:
	struct CreationOptions {
		uint32_t majorVersion = 7;
		uint32_t minorVersion = 4;
		ImageFormat outputFormat = FORMAT_DEFAULT;
		ImageConversion::ResizeMethod widthResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeMethod heightResizeMethod = ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR;
		Flags flags = FLAG_NONE;
		uint16_t initialFrameCount = 1;
		uint16_t startFrame = 0;
		bool isCubeMap = false;
		bool hasSphereMap = false;
		uint16_t initialSliceCount = 1;
		bool createMips = true;
		bool createThumbnail = true;
		bool createReflectivity = true;
		int16_t compressionLevel = -1;
		CompressionMethod compressionMethod = CompressionMethod::ZSTD;
		float bumpMapScale = 1.f;
	};

	/// This value is only valid when passed to VTF::create through CreationOptions
	static constexpr auto FORMAT_UNCHANGED = static_cast<ImageFormat>(-2);

	/// This value is only valid when passed to VTF::create through CreationOptions or VTF::setFormat
	static constexpr auto FORMAT_DEFAULT = static_cast<ImageFormat>(-1);

	static constexpr int32_t MAX_RESOURCES = 32;

	VTF();
	explicit VTF(std::vector<std::byte>&& vtfData, bool parseHeaderOnly = false);
	explicit VTF(std::span<const std::byte> vtfData, bool parseHeaderOnly = false);
	explicit VTF(const std::string& vtfPath, bool parseHeaderOnly = false);
	VTF(const VTF& other);
	VTF& operator=(const VTF& other);
	VTF(VTF&&) noexcept = default;
	VTF& operator=(VTF&&) noexcept = default;
	[[nodiscard]] explicit operator bool() const;
	static void create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, CreationOptions options);
	static void create(ImageFormat format, uint16_t width, uint16_t height, const std::string& vtfPath, CreationOptions options);
	[[nodiscard]] static VTF create(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, CreationOptions options);
	[[nodiscard]] static VTF create(ImageFormat format, uint16_t width, uint16_t height, CreationOptions options);
	static void create(const std::string& imagePath, const std::string& vtfPath, CreationOptions options);
	[[nodiscard]] static VTF create(const std::string& imagePath, CreationOptions options);
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
	[[nodiscard]] ImageFormat getFormat() const;
	void setFormat(ImageFormat newFormat, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR);
	[[nodiscard]] uint8_t getMipCount() const;
	bool setMipCount(uint8_t newMipCount);
	bool setRecommendedMipCount();
	void computeMips(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR);
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
	bool setImage(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0);
	bool setImage(const std::string& imagePath, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0);
	[[nodiscard]] std::vector<std::byte> saveImageToFile(uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;
	bool saveImageToFile(const std::string& imagePath, uint8_t mip = 0, uint16_t frame = 0, uint8_t face = 0, uint16_t slice = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)
	[[nodiscard]] bool hasThumbnailData() const;
	[[nodiscard]] std::span<const std::byte> getThumbnailDataRaw() const;
	[[nodiscard]] std::vector<std::byte> getThumbnailDataAs(ImageFormat newFormat) const;
	[[nodiscard]] std::vector<std::byte> getThumbnailDataAsRGBA8888() const;
	void setThumbnail(std::span<const std::byte> imageData_, ImageFormat format_, uint16_t width_, uint16_t height_);
	void computeThumbnail(ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR);
	void removeThumbnail();
	[[nodiscard]] std::vector<std::byte> saveThumbnailToFile(ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;
	bool saveThumbnailToFile(const std::string& imagePath, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)
	[[nodiscard]] std::vector<std::byte> bake() const;
	bool bake(const std::string& vtfPath) const; // NOLINT(*-use-nodiscard)
};
