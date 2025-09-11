#pragma once

#include "ImageConversion.h"

namespace vtfpp::ImageQuantize {

/// Converts a paletted image to something usable.
[[nodiscard]] std::vector<std::byte> convertP8ImageDataToBGRA8888(std::span<const std::byte> paletteData, std::span<const std::byte> imageData);

} // namespace vtfpp::ImageQuantize
