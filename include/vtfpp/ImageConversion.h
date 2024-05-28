#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "ImageFormats.h"

namespace vtfpp::ImageConversion {

[[nodiscard]] std::vector<std::byte> convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height);

/// Converts image data to a PNG or HDR file. HDR output will be used for floating-point formats.
[[nodiscard]] std::vector<std::byte> convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height);

} // namespace vtfpp::ImageConversion
