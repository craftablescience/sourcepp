#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "ImageConversion.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* vtfpp_ppl_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vtfpp_ppl_free
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create(uint32_t checksum);

// REQUIRES MANUAL FREE: vtfpp_ppl_free
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create_ex(uint32_t checksum, vtfpp_image_format_e format, uint32_t version);

// REQUIRES MANUAL FREE: vtfpp_ppl_free
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create_from_mem(const unsigned char* buffer, size_t bufferLen);

// REQUIRES MANUAL FREE: vtfpp_ppl_free
SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create_from_file(const char* pplPath);

SOURCEPP_API int vtfpp_ppl_is_valid(vtfpp_ppl_handle_t handle);

SOURCEPP_API uint32_t vtfpp_ppl_get_version(vtfpp_ppl_handle_t handle);

SOURCEPP_API void vtfpp_ppl_set_version(vtfpp_ppl_handle_t handle, uint32_t version);

SOURCEPP_API uint32_t vtfpp_ppl_get_checksum(vtfpp_ppl_handle_t handle);

SOURCEPP_API void vtfpp_ppl_set_checksum(vtfpp_ppl_handle_t handle, uint32_t checksum);

SOURCEPP_API vtfpp_image_format_e vtfpp_ppl_get_format(vtfpp_ppl_handle_t handle);

SOURCEPP_API void vtfpp_ppl_set_format(vtfpp_ppl_handle_t handle, vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_ppl_has_image_for_lod(vtfpp_ppl_handle_t handle, uint32_t lod);

// std::vector<uint32_t> getImageLODs() const;

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_raw(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, vtfpp_image_format_e format, uint32_t lod);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as_rgb888(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod);

class PPL {
public:
	struct Image {
		uint32_t width;
		uint32_t height;
		std::vector<std::byte> data;
	};

	bool setImage(std::span<const std::byte> imageData, ImageFormat format_, uint32_t width, uint32_t height, uint32_t lod = 0);

	bool setImage(std::span<const std::byte> imageData, ImageFormat format_, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR);

	bool setImage(const std::string& imagePath, uint32_t lod = 0);

	bool setImage(const std::string& imagePath, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::BILINEAR);

	[[nodiscard]] std::vector<std::byte> saveImageToFile(uint32_t lod = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const;

	bool saveImageToFile(const std::string& imagePath, uint32_t lod = 0, ImageConversion::FileFormat fileFormat = ImageConversion::FileFormat::DEFAULT) const; // NOLINT(*-use-nodiscard)

	[[nodiscard]] std::vector<std::byte> bake();

	bool bake(const std::string& pplPath);

protected:
	uint32_t version{};
	uint32_t checksum{};
	ImageFormat format{};

	std::unordered_map<uint32_t, Image> images;
};
