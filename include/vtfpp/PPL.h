#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "ImageConversion.h"

namespace vtfpp {

class PPL {
public:
	struct Image {
		uint32_t width;
		uint32_t height;
		std::vector<std::byte> data;
	};

	explicit PPL(uint32_t modelChecksum, ImageFormat format_ = ImageFormat::RGB888, uint32_t version_ = 0);

	explicit PPL(std::span<const std::byte> pplData);

	explicit PPL(const std::string& pplPath);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] uint32_t getVersion() const;

	void setVersion(uint32_t newVersion);

	[[nodiscard]] uint32_t getModelChecksum() const;

	void setModelChecksum(uint32_t newChecksum);

	[[nodiscard]] ImageFormat getFormat() const;

	void setFormat(ImageFormat newFormat);

	[[nodiscard]] bool hasImageForLOD(uint32_t lod) const;

	[[nodiscard]] std::vector<uint32_t> getImageLODs() const;

	[[nodiscard]] const Image* getImageRaw(uint32_t lod = 0) const;

	[[nodiscard]] std::optional<Image> getImageAs(ImageFormat newFormat, uint32_t lod = 0) const;

	[[nodiscard]] std::optional<Image> getImageAsRGB888(uint32_t lod = 0) const;

	bool setImage(std::span<const std::byte> imageData, ImageFormat format_, uint32_t width, uint32_t height, uint32_t lod = 0);

	bool setImage(std::span<const std::byte> imageData, ImageFormat format_, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

	bool setImage(const std::string& imagePath, uint32_t lod = 0);

	bool setImage(const std::string& imagePath, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod = 0, ImageConversion::ResizeFilter filter = ImageConversion::ResizeFilter::DEFAULT);

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

} // namespace vtfpp
